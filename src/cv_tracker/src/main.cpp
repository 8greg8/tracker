/*
 * Copyright (C) 2017 Gregor Koporec <gregor.koporec@gmail.com>, University of Ljubljana
 * Copyright (C) 2017 Janez Pers <janez.pers@fe.uni-lj.si>, University of Ljubljana
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// opencv
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>

// std
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <ctime>
#include <memory>

// posix
#include <wordexp.h> // for expanding ~ to $HOME directory

#include <boost/filesystem.hpp>

// local
#include "util/roi.hpp"
#include "time/videotimer.hpp"
#include "tracker/tracker.hpp"
#include "tracker/trackerfile.hpp"
#include "terminal/terminalparser.hpp"
#include "util/userinteraction.hpp"
#include "util/scaler.hpp"
#include "util/roiselector.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;
using namespace gk;

static const string DISPLAY_WINDOW_TRACKER = "Tracker";
static const int BOUNDING_BOX_THICKNESS = 2;


Rect2d roi;
Mat frame, image;


int main(int argc, char** argv){  
    TerminalParser terminalParser = TerminalParser(argc, argv, roi);
    
    gk::Tracker *tracker = NULL;
    tracker = terminalParser.getTracker();

    // Open video for optical flow estimation
    // If video is not opened end this program with error
    std::shared_ptr<VideoCapture> videoCapture =
            std::make_shared<VideoCapture>(terminalParser.getInputVideoname());
    if( !videoCapture->isOpened() ){
        cout << endl;
    	cout << "Could not open the input video: " << terminalParser.getInputVideoname() << endl;
        exit(EXIT_FAILURE);
    }

    // Get codec type - 4-char code for codec
    int codecNum = static_cast<int>(videoCapture->get(CAP_PROP_FOURCC));
    double fps = videoCapture->get(CAP_PROP_FPS);
    Size videoSize((int) videoCapture->get(CAP_PROP_FRAME_WIDTH),
    		(int) videoCapture->get(CAP_PROP_FRAME_HEIGHT));
    double frameCount = videoCapture->get(CAP_PROP_FRAME_COUNT);

    UserInteraction::printVideoProperties(
        videoSize, codecNum, fps, frameCount, terminalParser.getInputVideoname()
    );
    
    cout << "=======================" << endl;
    cout << "Starting tracking..." << endl;
    
    Mat scaledFrame;
    Mat flow, trackerFrame;
    Mat flowAngle, flowMagnitude;
    Mat opticalFlowImage, opticalFlowFrame;
    UMat ugray, uprevgray;
    double frameNumber;
    
    // For writing tracker roi to file
    TrackerFile trackerFile(terminalParser.getTrackedRoiFilename());

    bool constructDisplayWindow = true, constructDisplayWindowFlow = true;
    bool changeDisplayWindowImage = false, changeDisplayWindowImageFlow = false;
    
    VideoTimer timer(videoCapture, 1.0, terminalParser.getSelectionSeconds());
    timer.start();
    if(terminalParser.isSelectionEnabled() && 
            !terminalParser.needSelectRoiOnFirstFrame()){
        timer.startTimeToSelect();
    }
    
    for(;;)
    {
    	// Get new frame from video
        // If there are no more frames in video file stop for loop     
        if(!videoCapture->read(frame)){
            cout << endl;
            cout << "=======================" << endl;
            cout << "Frame " << videoCapture->get(CAP_PROP_POS_FRAMES)
                 << " is last frame." << endl;
            cout << "Video time at end: " << timer.getVideoTime() << endl;
            cout << "=======================" << endl;
            break;
        }
        frameNumber = videoCapture->get(CAP_PROP_POS_FRAMES);
        
        // Get gray frame for optical flow calculation
        if(frame.empty()){
            cerr << "Frame " << frameNumber << " is empty. Skipping it..." << endl;
            continue;
        }

        if(terminalParser.isTrackerScalingEnabled()){
            Scaler::scaleFrame(frame, frame, terminalParser.getTrackerDownScale());
        }

        // If previous gray not empty calculate optical flow
        if( frameNumber > 1 )
        {
            // Get part of tracked frame if tracker is enabled
            // Update tracker with downscaled frame and roi
            // If tracker found object, it is updated, so correct roi
            // And continue
            if(tracker->update(frame, roi)){
                Roi::correct<Rect2d>(roi, frame);
            }else{
                // Tracker didn't found tracked object
                // Roi should be empty.
                roi = Rect2d(0,0,0,0);
            }

            // If it is time to reselect ROI
            // select ROI and reinitialize tracker
            if(terminalParser.isSelectionEnabled()){
                if(timer.isTimeToSelect()){
                    
                    if(!RoiSelector::selectRoi(terminalParser.getTrackedRoiFilename(),
                            timer.getCompletion(), frame, roi)){
                        exit(EXIT_SUCCESS);
                    }
                    // Restart timer
                    timer.restartTimeToSelect();

                    // Init tracker with downscaled frame and roi.
                    // If subject is on a frame
                    if(roi.area() > 0){
                        tracker->init(frame, roi);
                    }
                }
            }              
            
            if(terminalParser.needDisplayTracker()){
                if(constructDisplayWindow){
                    namedWindow(DISPLAY_WINDOW_TRACKER, WINDOW_NORMAL);
                    constructDisplayWindow = false;
                    changeDisplayWindowImage = true;
                }
                if(changeDisplayWindowImage){
                    frame.copyTo(trackerFrame);
                    // Test if crop is working
                    //Roi::crop(frame, roi).copyTo(trackerFrame);
                    if(roi.area() > 0){
                        cv::rectangle(trackerFrame, roi, Scalar(255, 0, 0), 
                                BOUNDING_BOX_THICKNESS, 8, 0);
                    }
                    imshow(DISPLAY_WINDOW_TRACKER, trackerFrame);
                    int key = waitKey(1);
                    switch((char)key){
                       case 'q':
                           cout << endl;
                           cout << "You wanted to exit. Exiting..." << endl;
                           exit(EXIT_SUCCESS);
                           break;
                        case 'w':
                            cout << endl;
                            cout << "You don't want output to be shown..." << endl;
                            destroyWindow(DISPLAY_WINDOW_TRACKER);
                            changeDisplayWindowImage = false;
                            break;
                   }
                }
            }
            
            // Write normalized histogram to csv file
            trackerFile.write(roi);
             
        // If we have first frame and tracker enabled
        // we initialize region of interest or "roi"
        } else //if(videoCapture.get(CAP_PROP_POS_FRAMES) > 274309)
        {
            if(terminalParser.needSelectRoiOnFirstFrame()){
                // If we need to select ROI for the first time.
                if(!RoiSelector::selectRoi(terminalParser.getTrackedRoiFilename(),
                        timer.getCompletion(), frame, roi)){
                    exit(EXIT_SUCCESS);
                }
                // Restart timer
                timer.restartTimeToSelect();                    
            }

            // Init tracker with downscaled frame and roi.
            getBuildInformation();
            tracker->init(frame, roi);  
                
            // Note: If file existed, opened file will be erased (truncated)
            trackerFile.open(); 
        }

        // Show % for user
        if(timer.isTimeToShowOutput()){
            const char* completion = timer.getCompletion().c_str();
            printf("\rFPS: %s\tCompletion: %s %%\tVideo time: %.2f\tFrame: %.2f\tElapsed: %s\tEstimated: %s",
                    timer.getFps().c_str(),
                    timer.getCompletion().c_str(),
                    videoCapture->get(CAP_PROP_POS_MSEC)/1000.0,
                    videoCapture->get(CAP_PROP_POS_FRAMES),
                    timer.getElapsedTime()->c_str(), 
                    timer.getEstimatedTime().c_str()
            );
            fflush(stdout);       
        }
    }  
    trackerFile.close();
    
    cout << endl;
    cout << "Tracking is finnished." << endl;
    printf("Elapsed time: %s\n", timer.getElapsedTime()->c_str());
    cout << "Video time: " << timer.getVideoTime() << endl;
    cout << "Max fps: " << timer.getMaxFps() << endl;
    cout << "Is real time: " << timer.isRealTime() << endl;
    cout << "=========================================================" << endl;

    return 0;
}

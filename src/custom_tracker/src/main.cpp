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
//#include <opencv2/tracking.hpp>

// std
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <ctime>

// posix
#include <wordexp.h>
#include <opencv2/core/types_c.h> // for expanding ~ to $HOME directory

// OpenTLD
#include "tld/TLDUtil.h"
#include "tld/TLD.h"

// local
#include "util/roi.hpp"
#include "util/ccvconverter.hpp"
#include "time/videotimer.hpp"
#include "terminal/terminalparser2.hpp"
#include "util/userinteraction.hpp"
#include "util/dlibconverter.hpp"
#include "tracker/tracker.hpp"
#include "tracker/trackerfile.hpp"
#include "util/roiselector.hpp"

// libccv
extern "C"{
#include <ccv.h>
}

// dlib
#include <dlib/image_processing.h>
#include <dlib/opencv.h>


using namespace ::tld;
//using namespace cv;
using namespace std;
//using namespace dlib;
using namespace gk;

// Global 

Rect2d roiForTld, roiForKcf, roiForCcv, roiForDlib;
Rect roiForOpenTld, roi2;
Rect2d outputRoi;

static const string SELECTION_WINDOW_NAME = "Selection";
Mat frame;

static const int THICKNESS = 2;
static const Scalar tldColor(28, 26, 228); //red
static const Scalar kcfColor(184, 126, 55); //blue
static const Scalar ccvColor(74, 175, 77); //green
static const Scalar openTldColor(163, 78, 152); //purple
static const Scalar dlibColor(0, 127, 255); //orange

Rect2d roi;

int main(int argc, char** argv){
    TerminalParser2 terminalParser = TerminalParser2(argc, argv, roi);
    
    
    //Ptr<Tracker> tracker = cv::Tracker::create(trackerType);
    TLD* openTldTracker = new TLD();
    ccv_tld_t* ccvTracker;
    dlib::correlation_tracker dlibTracker;
    gk::Tracker *tldTracker = NULL;
    gk::Tracker *kcfTracker = NULL;
    
    bool selectRoiOnFirstFrame = false;
    double selectionSeconds = 0;
    bool isSelectionEnabled = false;
    if(terminalParser.tldTrackerEnabled){
        cout << "=======================" << endl;
        cout << "Initializing TLD tracker..." << endl;
        
        // Create tracker
        tldTracker = new gk::Tracker("TLD");

        // Get ROI from file
        if(terminalParser.isRoiFilename){            
            // Get ROI before any file was opened
            roiForTld = Roi::selectRect<Rect2d>(
                    terminalParser.roiFilename, 
                    terminalParser.input, 
                    terminalParser.playerId
                    );
            
            // If ROI was not defined in file show user a new window
            // to select new ROI
            if(roiForTld.area() <= 0){
                cout << "Region of interest was not defined in file." << endl;
                cout << "Selecting region of interest on a frame." << endl;
                selectRoiOnFirstFrame = true;
                
            } else{
                selectRoiOnFirstFrame = false;
                cout << "=======================" << endl;
            }
        } else{
            cout << "File with player bounding boxes was not specified" << endl;
            cout << "Selecting region of interest on a frame." << endl;
            selectRoiOnFirstFrame = true;
        }          
    } 
    if(terminalParser.kcfTrackerEnabled){
        cout << "=======================" << endl;
        cout << "Initializing KCF tracker..." << endl;

        // Create tracker
        kcfTracker = new gk::Tracker("KCF");

        // Get ROI from file
        if (terminalParser.isRoiFilename) {
            // Get ROI before any file was opened
            roiForKcf = Roi::selectRect<Rect2d>(
                    terminalParser.roiFilename,
                    terminalParser.input,
                    terminalParser.playerId
                    );

            // If ROI was not defined in file show user a new window
            // to select new ROI
            if (roiForKcf.area() <= 0) {
                cout << "Region of interest was not defined in file." << endl;
                cout << "Selecting region of interest on a frame." << endl;
                selectRoiOnFirstFrame = true;

            } else {
                selectRoiOnFirstFrame = false;
                cout << "=======================" << endl;
            }
        } else {
            cout << "File with player bounding boxes was not specified" << endl;
            cout << "Selecting region of interest on a frame." << endl;
            selectRoiOnFirstFrame = true;
        }         
    }
    
    
    // If video is not opened end this program with error
    std::shared_ptr<VideoCapture> videoCapture =
            std::make_shared<VideoCapture>(terminalParser.input);
    if( !videoCapture->isOpened() ){
        cout << endl;
    	cout << "Could not open the input video: " << terminalParser.input << endl;
        exit(EXIT_FAILURE);
    }

    // Get codec type - 4-char code for codec
    int codecNum = static_cast<int>(videoCapture->get(CAP_PROP_FOURCC));
    double fps = videoCapture->get(CAP_PROP_FPS);
    Size videoSize((int) videoCapture->get(CAP_PROP_FRAME_WIDTH),
    		(int) videoCapture->get(CAP_PROP_FRAME_HEIGHT));
    double frameCount = videoCapture->get(CAP_PROP_FRAME_COUNT);

    UserInteraction::printVideoProperties(
        videoSize, codecNum, fps, frameCount, terminalParser.input
    );

    cout << "=======================" << endl;
    cout << "Starting tracking..." << endl;
    
    
    
    
    Mat previousFrame;
    Mat grey;
    double nextFrameIndex, completion, previousCompletion;
    
    ccv_dense_matrix_t *ccvFrame = NULL, *previousCCVFrame = NULL;
    ccv_rect_t ccvRoi;
    ccv_tld_info_t info;
    ccv_comp_t box;
    
    dlib::drectangle dlibRoi;
    cv_image<bgr_pixel> dlibFrame;

    
    VideoWriter videoWriter;
    if(terminalParser.isVideo){
        videoWriter.open(terminalParser.output, CV_FOURCC('M','J','P','G'), 
                fps, videoSize);
                
        if (!videoWriter.isOpened())
        {
            cout  << "Could not open the output video for write: " 
                  << terminalParser.output << endl;
            cout << "Terminating..." << endl;
            return -1;
        }

        UserInteraction::printVideoProperties(videoSize, codecNum, fps, 0, 
                terminalParser.output);
    }
    cout << "=======================" << endl;
    cout << "Starting optical flow estimation..." << endl;
    
    if(terminalParser.trackerFile){
        terminalParser.trackerFile->open();
    }
    
    bool constructDisplayWindow = false;
    bool changeDisplayWindowImage = false;
    VideoTimer timer(videoCapture, 1.0);
    timer.start();
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
        
        // If previous gray not empty calculate optical flow
        if( videoCapture->get(CAP_PROP_POS_FRAMES) > 1 )
        {
            // Get part of tracked frame if tracker is enabled
            // Update tracking result
            //cvtColor(frame, grey, CV_BGR2GRAY);
            // Update tracker


            if(terminalParser.tldTrackerEnabled){
                if(tldTracker->update(frame, roiForTld)){
                    Roi::correct<Rect2d>(roiForTld, frame);
                    outputRoi = roiForTld;

                    

                    cv::rectangle(frame, roiForTld, tldColor, THICKNESS);
                    cv::putText(frame, "OPENCV-TLD", Point(roiForTld.x, roiForTld.y), 
                            FONT_HERSHEY_COMPLEX_SMALL, 1.0, tldColor);
                }
            }
            if (terminalParser.kcfTrackerEnabled) {
                if (kcfTracker->update(frame, roiForKcf)) {
                    Roi::correct<Rect2d>(roiForKcf, frame);
                    outputRoi = roiForKcf;
                    
                    cv::rectangle(frame, roiForKcf, kcfColor, THICKNESS);
                    cv::putText(frame, "KCF", Point(roiForKcf.x, roiForKcf.y),
                            FONT_HERSHEY_COMPLEX_SMALL, 1.0, kcfColor );
                }
            }

            if(terminalParser.ccvTrackerEnabled){
                CcvConverter::convertToCCVMat(frame, &ccvFrame);
                box = ccv_tld_track_object(ccvTracker, previousCCVFrame, ccvFrame, &info);
                CcvConverter::convertToRect(box.rect, roiForCcv);
                Roi::correct<Rect2d>(roiForCcv, frame);
                outputRoi = roiForCcv;
                
                cv::rectangle(frame, roiForCcv, ccvColor, THICKNESS);
                cv::putText(frame, "CCV-TLD", Point(roiForCcv.x, roiForCcv.y),
                        FONT_HERSHEY_COMPLEX_SMALL, 1.0, ccvColor);
            }

            if(terminalParser.opentldTrackerEnabled){
                openTldTracker->processImage(frame);
                if(openTldTracker->currBB != NULL){
                    roi2.x = openTldTracker->currBB->x;
                    roi2.y = openTldTracker->currBB->y;
                    roi2.width = openTldTracker->currBB->width;
                    roi2.height = openTldTracker->currBB->height;
                }
                Roi::correct<Rect>(roi2, frame);
                outputRoi = roi2;
                
                cv::rectangle(frame, roi2, openTldColor, THICKNESS);
                cv::putText(frame, "NEBEHAY-TLD", Point(roi2.x, roi2.y),
                        FONT_HERSHEY_COMPLEX_SMALL, 1.0, openTldColor);
            }
            
            if(terminalParser.dlibTrackerEnabled){
                DlibConverter::toDlibImage(frame, dlibFrame);
                dlibTracker.update(dlibFrame);
                
                DlibConverter::toRect(dlibTracker.get_position(), roiForDlib);
                Roi::correct<Rect2d>(roiForDlib, frame);
                outputRoi = roiForDlib;
                
                cv::rectangle(frame, roiForDlib, dlibColor, THICKNESS);
                cv::putText(frame, "CORR", Point(roiForDlib.x, roiForDlib.y),
                        FONT_HERSHEY_COMPLEX_SMALL, 1.0, dlibColor);
            }
          
            // Extract roi
            ///frame = extractROI(frame, roi);
            ///frame = extractROI(frame, roi2);
            if(constructDisplayWindow){
                namedWindow("Display Window", WINDOW_NORMAL);
                constructDisplayWindow = false;
                changeDisplayWindowImage = true;
            }
            if(changeDisplayWindowImage){
                imshow("Display Window", frame);
                int key = waitKey(1);
                switch((char)key){
                   case 'q':
                       cout << "You wanted to exit. Exiting..." << endl;
                       exit(EXIT_SUCCESS);
                       break;
                    case 'w': // space key was pressed
                        cout << "You don't want output to be shown..." << endl;
                        destroyWindow("Display Window");
                        changeDisplayWindowImage = false;
                        break;
               }
            }
                
                
                
                if(terminalParser.isVideo){
                    videoWriter << frame;
                }
                if(terminalParser.trackerFile){
                    terminalParser.trackerFile->write(outputRoi);
                }
                
                
                
                
                
            // Something went wrong because sizes are different.
            if(previousFrame.size() != frame.size()){
                cout << "Something went wrong because sizes of previous"
                     << "frame and current frame are different" << endl;
                cout << "Maybe the problem is in video." << endl;
                cout << "Previous frame size: " 
                     << previousFrame.rows << "x" << previousFrame.cols << endl;
                cout << "Current frame size: "
                     << frame.rows << "x" << frame.cols << endl;
                cout << "Current timestamp in ms: "
                     << videoCapture->get(CAP_PROP_POS_MSEC) << endl;
                cout << "Next frame: "
                     << videoCapture->get(CAP_PROP_POS_FRAMES) << endl;
                cout << "Exiting..." << endl;
                if(terminalParser.trackerFile){
                    terminalParser.trackerFile->close();
                }
                exit(-1);
            }
            
           
        // If we have first frame and tracker enabled
        // we initialize region of interest or "roi"
        } else{
                if(terminalParser.tldTrackerEnabled){
                    if(selectRoiOnFirstFrame){
                        // If we need to select ROI for the first time.
                        if (!RoiSelector::selectRoi(terminalParser.roiFilename,
                                timer.getCompletion(), frame, roi)) {
                            exit(EXIT_SUCCESS);
                        }
                    }
                    // Quit if ROI was not selected
                    if(roiForTld.width == 0 || roiForTld.height == 0){
                        cout << "Region of interest was not selected." << endl;
                        cout << "Exiting..." << endl;
                        exit(-1);
                    }

                    // Initialize tracker
                    tldTracker->init(frame, roiForTld);
                }
                
                if (terminalParser.kcfTrackerEnabled) {
                    if (selectRoiOnFirstFrame) {
                        // If we need to select ROI for the first time.
                        if (!RoiSelector::selectRoi(terminalParser.roiFilename,
                                timer.getCompletion(), frame, roi)) {
                            exit(EXIT_SUCCESS);
                        }
                    }
                    // Quit if ROI was not selected
                    if (roiForKcf.width == 0 || roiForKcf.height == 0) {
                        cout << "Region of interest was not selected." << endl;
                        cout << "Exiting..." << endl;
                        exit(EXIT_FAILURE);
                    }

                    // Initialize tracker
                    kcfTracker->init(frame, roiForKcf);
                }
                
                if(terminalParser.opentldTrackerEnabled){
                    roiForOpenTld = Roi::selectRect<Rect>(
                            terminalParser.roiFilename, 
                            terminalParser.input, 
                            terminalParser.playerId);
                    if(roiForOpenTld.width == 0 || roiForOpenTld.height == 0){
                        cout << "Region of interest was not selected." << endl;
                        cout << "Exiting..." << endl;
                        exit(-1);
                    }
                    
                    cvtColor(frame, grey, CV_BGR2GRAY);
                    
                    openTldTracker->detectorCascade->imgWidth = grey.cols;
                    openTldTracker->detectorCascade->imgHeight = grey.rows;
                    openTldTracker->detectorCascade->imgWidthStep = grey.step;
                    openTldTracker->selectObject(grey, &roiForOpenTld);
                }
                
                if(terminalParser.ccvTrackerEnabled){
                    roiForCcv = Roi::selectRect<Rect>(
                        terminalParser.roiFilename,
                        terminalParser.input,
                        terminalParser.playerId);
                    if(roiForCcv.width == 0 || roiForCcv.height == 0){
                        cout << "Region of interest was not selected." << endl;
                        cout << "Exiting..." << endl;
                        exit(-1);
                    }
                    
                    CcvConverter::convertToCCVMat(frame, &ccvFrame);
                    CcvConverter::convertToCCVRect(roiForCcv, ccvRoi);
                    ccv_tld_param_t param = ccv_tld_default_params;
                    //param.rotation = 1;
                    ccvTracker = ccv_tld_new(ccvFrame, ccvRoi, param);
                }
                
                if(terminalParser.dlibTrackerEnabled){
                    roiForDlib = Roi::selectRect<Rect>(
                        terminalParser.roiFilename,
                        terminalParser.input,
                        terminalParser.playerId);
                    if(roiForDlib.width == 0 || roiForDlib.height == 0){
                        cout << "Region of interest was not selected." << endl;
                        cout << "Exiting..." << endl;
                        exit(-1);
                    }
                    
                    DlibConverter::toDlibRectangle(roiForDlib, dlibRoi);
                    DlibConverter::toDlibImage(frame, dlibFrame);
                    dlibTracker.start_track(dlibFrame, dlibRoi);
                }
                
                // Extract roi from frame
                //frame = extractROI(frame, roi);
                //frame = extractROI(frame, roi1);
                //rectangle(frame, roi, Scalar(255, 0, 0));
                
                //imshow("Test", frame);
                //waitKey(0);
                
    
        }
        //timer.incrementFrameCount();

        // Make current gray previous gray
        std::swap(previousFrame, frame);
        
        if(terminalParser.ccvTrackerEnabled){
            previousCCVFrame = ccvFrame;
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
    if(terminalParser.trackerFile){
        terminalParser.trackerFile->close();
    }
    if(tldTracker){
        delete tldTracker;
    }
    if(kcfTracker){
        delete kcfTracker;
    }
    if(openTldTracker){
        delete openTldTracker;
    }
   
    cout << endl;
    cout << "Tracking is finnished." << endl;
    printf("Elapsed time: %s\n", timer.getElapsedTime()->c_str());
    cout << "Video time: " << timer.getVideoTime() << endl;
    cout << "Max fps: " << timer.getMaxFps() << endl;
    cout << "Is real time: " << timer.isRealTime() << endl;
    cout << "=========================================================" << endl;
   /**/
    return 0;
}

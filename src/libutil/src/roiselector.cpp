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

#include "util/roiselector.hpp"

using namespace gk;

const string RoiSelector::SELECTION_WINDOW_NAME = "Selection";
const int RoiSelector::BOUNDING_BOX_THICKNESS = 2;

string RoiSelector::snapshotFilename;
string RoiSelector::windowTitle;
bool RoiSelector::startSelection = false;
bool RoiSelector::isObjectSelected = false;
cv::Mat RoiSelector::frame;
cv::Mat RoiSelector::image;
cv::Rect RoiSelector::selectedRoi;
cv::Rect RoiSelector::oldRoi;
cv::Point RoiSelector::startSelectionPoint;
cv::Point RoiSelector::endSelectionPoint;

void RoiSelector::onMouse(int event, int x, int y, int, void*) {
    
    if (event == CV_EVENT_LBUTTONDOWN && !startSelection) {
        // Left button clicked
        startSelectionPoint = cv::Point(x, y);
        startSelection = true;

    } else if (event == CV_EVENT_RBUTTONDOWN && !startSelection) {
        // Right button clicked
        if (oldRoi.area() > 0) {
            frame.copyTo(image);
            rectangle(image, oldRoi, cv::Scalar(0, 255, 0),
                    BOUNDING_BOX_THICKNESS, 8, 0);
            imshow(windowTitle, image);
            startSelection = false;
        }

    } else if (event == CV_EVENT_MOUSEMOVE && startSelection) {
        // Mouse dragged
        frame.copyTo(image);
        endSelectionPoint = cv::Point(x, y);
        rectangle(image, startSelectionPoint,
                endSelectionPoint, cv::Scalar(0, 0, 255),
                BOUNDING_BOX_THICKNESS, 8, 0);
        imshow(windowTitle, image);

    } else if (event == CV_EVENT_LBUTTONUP) {
        endSelectionPoint = cv::Point(x, y);
        selectedRoi = Rect2d(startSelectionPoint.x, startSelectionPoint.y,
                endSelectionPoint.x - startSelectionPoint.x,
                endSelectionPoint.y - startSelectionPoint.y);
        Roi::correct(selectedRoi, frame);
        startSelection = false;
    }

}

bool RoiSelector::selectRoi(const string& trackedRoiFilename, const string& completion, cv::Mat& frame, cv::Rect2d& roi) {
    startSelection = false;
    isObjectSelected = false;
    
    windowTitle = SELECTION_WINDOW_NAME + " " + completion + " %";
    snapshotFilename = getSnapshotFilename(trackedRoiFilename, completion);
    
    RoiSelector::frame = frame.clone();
    RoiSelector::frame.copyTo(image);
    
    RoiSelector::oldRoi = roi;

    cv::namedWindow(windowTitle, cv::WINDOW_NORMAL);
    cv::setMouseCallback(windowTitle, onMouse, 0);
    bool quit = false;
    
    while (isObjectSelected == false && quit == false) {
        // If ROI already exists, show it as green box and make it as selection
        if (roi.area() > 0) {
            cv::rectangle(image, roi, cv::Scalar(0, 255, 0), BOUNDING_BOX_THICKNESS, 8, 0);
            RoiSelector::selectedRoi = Rect2d(roi.x, roi.y, roi.width, roi.height);

        } else {
            // If ROI didn't exist.
            RoiSelector::selectedRoi = Rect2d();
        }

        cv::imshow(windowTitle, image);
        char key = (char) cv::waitKey(0);

        switch (key) {
            case 'q':
                cout << endl;
                cout << "You wanted to exit. Exiting..." << endl;
                quit = true;
                break;
            case 'n':
                cout << endl;
                cout << "You said no subject on a frame." << endl;
                RoiSelector::selectedRoi.x = 0;
                RoiSelector::selectedRoi.y = 0;
                RoiSelector::selectedRoi.width = 0;
                RoiSelector::selectedRoi.height = 0;
                isObjectSelected = checkTerminationCriteria(windowTitle, RoiSelector::selectedRoi);
                break;
            case 's':
                cout << endl;
                cout << "Frame was saved to file " + snapshotFilename << endl;
                imwrite(snapshotFilename, image);
                break;
            default:
                isObjectSelected = checkTerminationCriteria(windowTitle, RoiSelector::selectedRoi);
                break;
        }
    }

    cv::setMouseCallback(windowTitle, 0, 0);
    cv::destroyWindow(windowTitle);
    
    if(quit){
        return false;
    } else{
        roi = RoiSelector::selectedRoi;
        // Show user
        cout << endl;
        cout << "New region of interest was selected: " << endl;
        cout << "x: " << cvRound(roi.x) << "\ty: " << cvRound(roi.y) << "\twidth: ";
        cout << cvRound(roi.width) << "\theight: " << cvRound(roi.height) << endl;

        return true;
    }
}

string RoiSelector::getSnapshotFilename(const string& trackedRoiFilename, const string& completion) {
    boost::filesystem::path parentPath(trackedRoiFilename);
    boost::filesystem::path directory = parentPath.parent_path();
    return directory.string() + "/" + "snapshot-selection" + "-" + completion + ".png";
}

bool RoiSelector::checkTerminationCriteria(const string& windowName, const cv::Rect& selectedRoi) {
    if (selectedRoi.width < 0 || selectedRoi.height < 0) {
        cout << endl;
        cout << "You must drag from top left to bottom right corner." << endl;
        cout << "Please do it again." << endl;
        return false;

    } else {
        return true;
    }
}


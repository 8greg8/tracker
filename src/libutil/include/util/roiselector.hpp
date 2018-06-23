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

#ifndef ROISELECTOR_HPP
#define ROISELECTOR_HPP

#include <iostream>

#include <boost/filesystem.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "util/roi.hpp"


using namespace std;

namespace gk {

    class RoiSelector {
    private:
        static const string SELECTION_WINDOW_NAME;
        static const int BOUNDING_BOX_THICKNESS;

        static bool startSelection;
        static bool isObjectSelected;
        static cv::Rect selectedRoi;
        static cv::Rect oldRoi;
        static cv::Mat frame;
        static cv::Mat image;
        static std::string windowTitle;
        static cv::Point startSelectionPoint;
        static cv::Point endSelectionPoint;
        static string snapshotFilename;

        static string getSnapshotFilename(const string& trackedRoiFilename, const string& completion);
        static void onMouse(int event, int x, int y, int, void*);
        static bool checkTerminationCriteria(const std::string& windowName, const cv::Rect& selectedRoi);

    public:
        static bool selectRoi(const string& trackedRoiFilename, const string& completion, cv::Mat& frame, cv::Rect2d& roi);


    };
}
#endif /* ROISELECTOR_HPP */


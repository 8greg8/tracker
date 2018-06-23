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

#ifndef TERMINALPARSER_HPP
#define TERMINALPARSER_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <iostream>

// posix
#include <wordexp.h> // for expanding ~ to $HOME directory

// boost
#include <boost/format.hpp>

#include "tracker/tracker.hpp"
#include "util/roi.hpp"
#include "util/scaler.hpp"

using namespace std;
using namespace boost;

namespace gk{   
    class TerminalParser{
    private:
        // Start with 2nd frame. First frame doesnt hanve optical flow.
        static int flowFrameCount; 
        static const String KEYS;
        
        CommandLineParser parser;

        void initTracker(Rect2d& roi);
        string expandName(string input);
        string parseMainInput(string input);
        
        
        /* FIELDS */
        string inputVideoname;
        string trackedRoiFilename;
        
        int frameNumber;
        bool needImage;
        
        gk::Tracker *tracker;
        bool trackerEnabled;
        bool selectRoiOnFirstFrame = false;
        double selectionSeconds = 0;
        bool selectionEnabled = false;
        bool displayTracker;
        bool trackerScaling = false;
        double trackerScale;

    public:
        TerminalParser(int argc, const char *const argv[], Rect2d& roi);
        
        void help();
    
        string getInputVideoname();
        string getTrackedRoiFilename();
        gk::Tracker* getTracker();
        double getSelectionSeconds();
        bool isSelectionEnabled();
        bool needSelectRoiOnFirstFrame();
        int getFrameNumber();
        bool needDisplayTracker();
        bool isTrackerScalingEnabled();
        double getTrackerDownScale();
        double getTrackerUpScale();
        
    };
}

#endif /* TERMINALPARSER_HPP */


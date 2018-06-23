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

#include "terminal/terminalparser.hpp"

using namespace gk;

gk::TerminalParser::TerminalParser(int argc, const char *const argv[], Rect2d& roi) : 
parser(argc, argv, KEYS){
    tracker = NULL;
    
    // Other inputs
    inputVideoname = parseMainInput(parser.get<string>("v"));
    trackedRoiFilename = parseMainInput(parser.get<string>("csv"));
    
    initTracker(roi);
    
    // If user wants help
    if (parser.has("help")){
        help();
        exit(EXIT_SUCCESS);
    }
    
    displayTracker = parser.get<bool>("displayTracker");
}

// Parse input string
const String TerminalParser::KEYS =
"{help h usage ? |        | Print this message             }"
"{v                 | <none> | Input video for optical flow   }"
"{csv               | <none> | CSV file for appending features}"
"{t                 | KCF    | Enabling tracker with specific type }"
"{roi               |        | File with player bounding boxes}"
"{n                 | 1      | Player ID to track             }"
"{select            |        | Select ROI on every N seconds. }"
"{displayTracker    | false  | If video with tracker is shown.}"
"{ts trackerScale   |        | Factor for scaling video when using tracker.}"
            
;//End parse key

void TerminalParser::help(){
    cout << "tracker v1.0" << endl;
    cout << TerminalParser::KEYS << endl;
    cout <<
    "\nThis program tracks subject in a video\n"
    "Call:\n"
    "tracker "
    "{help | h | usage | ?} "
    "{-[parameter name]=[parameter value]} "
    "\n"
    "Parameters:\n"
    "\t-v: must have\n"
    "\t-csv: must have\n"
    "\t-t: optional\n"
        "\t\tTLD\n"
        "\t\tMIL\n"
        "\t\tBOOSTING\n"
        "\t\tMEDIANFLOW\n"
        "\t\tKCF (default)\n"
    "\t-roi: optional\n"
    "\t-n: must have for roi. Defalut 1\n"
    "\t-select: optional\n"
    "\t-displayTracker: optional\n"
    "\t-ts | trackerScale: optional\n"
            << endl;
}

void TerminalParser::initTracker(Rect2d& roi){
    cout << "=======================" << endl;
    cout << "Initializing tracker..." << endl;
    
    // Create tracker
    string trackerType = parser.get<string>("t");
    tracker = new gk::Tracker(trackerType);
    
    // Check for tracker scale
    if(parser.has("ts")){
        trackerScaling = true;
        trackerScale = parser.get<double>("ts");
    } else{
        trackerScaling = false;
    }

    // Get ROI from file
    if(parser.has("roi")){
        string roiFilename = parser.get<string>("roi");
        int playerId = parser.get<int>("n");

        // Get ROI before any file was opened
        roi = Roi::selectRect<Rect2d>(roiFilename, inputVideoname, playerId);

        // If ROI was not defined in file show user a new window
        // to select new ROI
        if(roi.area() <= 0){
            cout << "Region of interest was not defined in file." << endl;
            cout << "Selecting region of interest on a frame." << endl;
            selectRoiOnFirstFrame = true;

        } else{
            // Downscale ROI if tracker scaling is used.
            if(trackerScaling){
                Scaler::scaleRoi(roi, roi, getTrackerDownScale());
            }
            selectRoiOnFirstFrame = false;
            cout << "=======================" << endl;
        }
    } else{
        cout << "File with player bounding boxes was not specified" << endl;
        cout << "Selecting region of interest on a frame." << endl;
        selectRoiOnFirstFrame = true;
    }
    
    // Check for selection
    if(parser.has("select")){
        selectionSeconds = parser.get<double>("select");
        if(selectionSeconds > 0){
            selectionEnabled = true;
        }
    }
}


string TerminalParser::expandName(string input){
    if(input == ""){
        return input;
    }
    
    wordexp_t nameExpansion;
    wordexp(input.c_str(), &nameExpansion, 0);
    
    string expandedName = nameExpansion.we_wordv[0];
    wordfree(&nameExpansion);
    
    return expandedName;
}

string TerminalParser::parseMainInput(string input){
    string expandedName = expandName(input);
    
    if(expandedName.empty()){
        help();
        exit(EXIT_FAILURE);
    } else{
        return expandedName;
    }
}

string TerminalParser::getInputVideoname(){
    return inputVideoname;
}

string TerminalParser::getTrackedRoiFilename(){
    return trackedRoiFilename;
}

gk::Tracker* TerminalParser::getTracker(){
    return tracker;
}

double TerminalParser::getSelectionSeconds(){
    return selectionSeconds;
}

bool TerminalParser::isSelectionEnabled(){
    return selectionEnabled;
}

bool TerminalParser::needSelectRoiOnFirstFrame(){
    return selectRoiOnFirstFrame;
}

int TerminalParser::getFrameNumber(){
    return frameNumber;
}

bool TerminalParser::needDisplayTracker(){
    return displayTracker;
}

bool TerminalParser::isTrackerScalingEnabled(){
    return trackerScaling;
}

double TerminalParser::getTrackerDownScale(){
    return trackerScale;
}

double TerminalParser::getTrackerUpScale(){
    return (1.0/trackerScale);
}

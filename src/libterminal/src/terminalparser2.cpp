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

#include "terminal/terminalparser2.hpp"

using namespace gk;

gk::TerminalParser2::TerminalParser2(int argc, const char *const argv[], Rect2d& roi) : 
parser(argc, argv, KEYS){
    
    // Other inputs
    input = parseMainInput(parser.get<string>("i"));
    isRoiFilename = false;
    if (parser.has("f")) {
        roiFilename = parseMainInput(parser.get<string>("f"));
        isRoiFilename = true;
    }
    int playerId = parser.get<int>("n");
    
    isVideo = false;
    if (parser.has("o")) {
        output = parseMainInput(parser.get<string>("o"));
        isVideo = true;
    }
    
    isRoiFile = false;
    if (parser.has("csv")) {
        roiFile = parseMainInput(parser.get<string>("csv"));
        isRoiFile = true;
    }
    
    tldTrackerEnabled = parser.has("tld");
    kcfTrackerEnabled = parser.has("kcf");
    opentldTrackerEnabled = parser.has("gn");
    ccvTrackerEnabled = parser.has("ccv");
    dlibTrackerEnabled = parser.has("dlib");
    

    if (isRoiFile) {
        trackerFile = new TrackerFile(roiFile);
    }
    
    //initTracker(roi);
    
    // If user wants help
    if (parser.has("help")){
        help();
        exit(EXIT_SUCCESS);
    }   
}

// Parse input string
const String TerminalParser2::KEYS =
"{help h usage ? | | Print this message.}"
"{i | <none> | Input video.}"
"{o | <none> | Output video name.}"
"{csv | | Output csv bounding box file.}"
"{f | <none> | File with player bounding boxes.}"
"{n | 1 | Player ID to track.}"
"{tld | |}"
"{kcf | |}"
"{gn | |}"
"{ccv | |}"
"{dlib | |}"
; //End parse key

void TerminalParser2::help(){
    cout <<
            "\nThis program generates video with tracker.\n"
            "Call:\n"
            << TerminalParser2::KEYS
            << endl;
}

void TerminalParser2::initTracker(Rect2d& roi){
    cout << "=======================" << endl;
    cout << "Initializing tracker..." << endl;
    
    
}


string TerminalParser2::expandName(string input){
    if(input == ""){
        return input;
    }
    
    wordexp_t nameExpansion;
    wordexp(input.c_str(), &nameExpansion, 0);
    
    string expandedName = nameExpansion.we_wordv[0];
    wordfree(&nameExpansion);
    
    return expandedName;
}

string TerminalParser2::parseMainInput(string input){
    string expandedName = expandName(input);
    
    if(expandedName.empty()){
        help();
        exit(EXIT_FAILURE);
    } else{
        return expandedName;
    }
}

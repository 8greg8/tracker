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

#include "tracker/trackerfile.hpp"

using namespace gk;

TrackerFile::TrackerFile(const string& filename) : filename(filename){}


void TrackerFile::open(){
    cout << endl;
    cout << "Erasing file " << filename << " for new data to write..." << endl;
    outputStream.open(filename);
}

void TrackerFile::close(){
    outputStream.close();
}

void TrackerFile::write(const Rect2d& roi){
    outputStream << roi.x << ","
                 << roi.y << ","
                 << roi.width << ","
                 << roi.height << endl;
}

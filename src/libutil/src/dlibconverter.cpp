/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "util/dlibconverter.hpp"

void DlibConverter::toDlibImage(const Mat& image, cv_image<bgr_pixel>& dlibImage) {
    dlibImage = cv_image<bgr_pixel>(image);
}

void DlibConverter::toDlibRectangle(const Rect2d& rectangle, dlib::drectangle& dlibRectangle) {
    dlibRectangle = drectangle(rectangle.x, rectangle.y, rectangle.x + rectangle.width, rectangle.y + rectangle.height);
}

void DlibConverter::toRect(const dlib::drectangle& input, Rect2d& output) {
    output = Rect2d(input.left(), input.top(), input.width(), input.height());
}



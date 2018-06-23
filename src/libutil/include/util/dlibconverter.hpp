/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dlibconverter.hpp
 * Author: gregork
 *
 * Created on October 23, 2016, 5:00 PM
 */

#ifndef DLIBCONVERTER_HPP
#define DLIBCONVERTER_HPP

#include <opencv2/core.hpp>
#include <dlib/opencv.h>
#include <dlib/geometry.h>

using namespace dlib;
using namespace cv;

class DlibConverter{
private:
    DlibConverter(){}
public:
    static void toDlibImage(const Mat& image, cv_image<bgr_pixel>& dlibImage);
    
    static void toDlibRectangle(const Rect2d& rectangle, dlib::drectangle& dlibRectangle);
    
    static void toRect(const dlib::drectangle& input, Rect2d& output);
};

#endif /* DLIBCONVERTER_HPP */


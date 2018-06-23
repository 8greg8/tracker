/*
 * ccvconverter.cpp
 *
 *  Created on: Oct 17, 2016
 *      Author: Gregor Koporec
 *     Version: 1.0
 */
#include "util/ccvconverter.hpp"

void CcvConverter::convertToMat(const ccv_dense_matrix_t& input, Mat& output){
    //Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP);
    output = Mat(
            input.rows, 
            input.cols, 
            getType(input.type), 
            input.data.u8, 
            input.step
    );
}
    

void CcvConverter::convertToCCVMat(const Mat& input, ccv_dense_matrix_t** output){
    ///ccv_matrix_t mat;// = ccv_matrix(input.rows, input.cols, getCCVType(input.type()), input.data, 0);
    //&output = ccv_get_dense_matrix(&mat)
    //output = ccv_dense_matrix(input.rows, input.cols, getCCVType(input.type()), input.data, 0);
    //output->data.u8 = input.data;
    //ccv_matrix_t* mat;
    //ccv_visualize(&output, &mat, getCCVType(input.type()));
    ccv_read(input.data, output, CCV_IO_BGR_RAW | CCV_IO_GRAY, input.rows, input.cols, input.step);
    //ccv_write(mat, "/home/gregork/test.png", 0, CCV_IO_PNG_FILE, 0);
}
    

void CcvConverter::convertToRect(const ccv_rect_t& input, Rect2d& output){
    output = Rect2d(input.x, input.y, input.width, input.height);
}
    

void CcvConverter::convertToCCVRect(const Rect2d& input, ccv_rect_t& output){
    output = ccv_rect(input.x, input.y, input.width, input.height);
}

int CcvConverter::getCCVType(int cvType){
    int type = 0;
    switch(cvType){
        case CV_8UC1:
            type = CCV_8U | CCV_C1;
            break;
        case CV_8UC3:
            type = CCV_8U | CCV_C3;
            break;
        case CV_32SC1:
            type = CCV_32S | CCV_C1;
            break;
        case CV_32SC3:
            type = CCV_32S | CCV_C3;
            break;
        case CV_32FC1:
            type = CCV_32F | CCV_C1;
            break;
        case CV_32FC3:
            type = CCV_32F | CCV_C3;
            break;
        case CV_64FC1:
            type = CCV_64F | CCV_C1;
            break;
        case CV_64FC3:
            type = CCV_64F | CCV_C3;
            break;
        default:
            type = 0;
            break;
    }
    return type;
}
    

int CcvConverter::getType(int ccvType){
    int type = 0;
    switch(ccvType){
        case CCV_8U | CCV_C1:
            type = CV_8UC1;
            break;
        case CCV_8U | CCV_C3:
            type = CV_8UC3;
            break;
        case CCV_32S | CCV_C1:
            type = CV_8UC1;
            break;
        case CCV_32S | CCV_C3:
            type = CV_8UC3;
            break;
        case CCV_32F | CCV_C1:
            type = CV_8UC1;
            break;
        case CCV_32F | CCV_C3:
            type = CV_8UC3;
            break;
        case CCV_64F | CCV_C1:
            type = CV_8UC1;
            break;
        case CCV_64F | CCV_C3:
            type = CV_8UC3;
            break;
        default:
            type = 0;
            break;
    }
    return type;
}

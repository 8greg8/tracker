/**
 * License 
 */

/* 
 * File:   ccvconverter.hpp
 * Author: gregork
 *
 * Created on October 17, 2016, 11:22 AM
 */

#ifndef CCVCONVERTER_HPP
#define CCVCONVERTER_HPP

#include <opencv2/core.hpp>
extern "C"{
#include <ccv.h>
}

using namespace cv;

class CcvConverter{
private:
    /**
     * Private constructor.
     */
    CcvConverter(){}
    
public:
    /**
     * Method for converting ccv matrix to opencv matrix.
     * 
     * @param input CCV matrix.
     * @param output OpenCV matrix.
     */
    static void convertToMat(const ccv_dense_matrix_t& input, Mat& output);
    
    /**
     * Method for converting from opencv matrix to ccv matrix.
     * 
     * @param input OpenCV matrix
     * @param output CCV matrix.
     */
    static void convertToCCVMat(const Mat& input, ccv_dense_matrix_t** output);
    
    /**
     * Method for converting ccv rectangle to opencv rectangle.
     * 
     * @param input CCV rectangle
     * @param output OpenCV rectangle
     */
    static void convertToRect(const ccv_rect_t& input, Rect2d& output);
    
    /**
     * Method for converting opencv rectangle to ccv rectangle.
     * 
     * @param input OpenCV rectangle.
     * @param output CCV rectangle.
     */
    static void convertToCCVRect(const Rect2d& input, ccv_rect_t& output);
    
    /**
     * Method for getting CCV matrix type from opencv matrix type.
     * 
     * @param input OpenCV matrix type.
     * @return CCV matrix type.
     */
    static int getCCVType(int cvType);
    
    /**
     * Method for getting opencv matrix type from ccv matrix type.
     * 
     * @param ccvType CCV matrix type.
     * @return OpenCV matrix type.
     */
    static int getType(int ccvType);
};

#endif /* CCVCONVERTER_HPP */


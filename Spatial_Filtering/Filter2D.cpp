/** Implementation file for image filtering
 *
 *  \file ipcv/spatial_filtering/Filter2D.cpp
 *  \author <Allison Hazebouck> (<amh7966>)
 *  \date 10 21 2019
 */

#include "Filter2D.h"

#include <iostream>

using namespace std;

namespace ipcv {

/** Correlates an image with the provided kernel
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[out] dst         destination cv::Mat of ddepth type
 *  \param[in] ddepth       desired depth of the destination image
 *  \param[in] kernel       convolution kernel (or rather a correlation
 *                          kernel), a single-channel floating point matrix
 *  \param[in] anchor       anchor of the kernel that indicates the relative
 *                          position of a filtered point within the kernel;
 *                          the anchor should lie within the kernel; default
 *                          value (-1,-1) means that the anchor is at the
 *                          kernel center
 *  \param[in] delta        optional value added to the filtered pixels
 *                          before storing them in dst
 *  \param[in] border_mode  pixel extrapolation method
 *  \param[in] border_value value to use for constant border mode
 */
bool Filter2D(const cv::Mat& src, cv::Mat& dst, const int ddepth,
              const cv::Mat& kernel, const cv::Point anchor, const int delta,
              const BorderMode border_mode, const uint8_t border_value) {

  // create destination matrix
  dst.create(src.rows, src.cols, CV_8UC3);
  // initialize variables
  int kernel_sum_r, kernel_sum_g, kernel_sum_b;
  int offset = kernel.rows / 2;
  int rows, cols, kr, kc, src_rows, src_cols;

  //loop through rows and cols of source and kernel
  for (rows = offset; rows < src.rows-offset; rows++) {
    for (cols = offset; cols < src.cols-offset; cols++) {
      kernel_sum_r = 0;
      kernel_sum_g = 0;
      kernel_sum_b = 0;
      for (kr = 0; kr < kernel.rows; kr++) {
        for (kc = 0; kc < kernel.cols; kc++) {
	  //sum multiplications at all points of kernel
          src_rows = rows + kr - offset;
          src_cols = cols + kc - offset;
          kernel_sum_r += kernel.at<float>(kr, kc) *
                          src.at<cv::Vec3b>(src_rows, src_cols)[0];
          kernel_sum_g += kernel.at<float>(kr, kc) *
                          src.at<cv::Vec3b>(src_rows, src_cols)[1];
          kernel_sum_b += kernel.at<float>(kr, kc) *
                          src.at<cv::Vec3b>(src_rows, src_cols)[2];
        }
      }
      //clip and crush as needed
      if (kernel_sum_r < 0) {
        kernel_sum_r = 0;
      } else if (kernel_sum_r > 255) {
        kernel_sum_r = 255;
      }
      if (kernel_sum_g < 0) {
        kernel_sum_g = 0;
      } else if (kernel_sum_g > 255) {
        kernel_sum_g = 255;
      }
      if (kernel_sum_b < 0) {
        kernel_sum_b = 0;
      } else if (kernel_sum_b > 255) {
        kernel_sum_b = 255;
      }
      //put summed values into destination mat
      dst.at<cv::Vec3b>(rows, cols)[0] = kernel_sum_r + delta;
      dst.at<cv::Vec3b>(rows, cols)[1] = kernel_sum_g + delta;
      dst.at<cv::Vec3b>(rows, cols)[2] = kernel_sum_b + delta;
    }
  }

  return true;
}
}

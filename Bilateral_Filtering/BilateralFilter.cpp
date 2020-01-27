/** Implementation file for bilateral filtering
 *
 *  \file ipcv/bilateral_filtering/BilateralFilter.cpp
 *  \author <Allison Hazebrouck> (<amh7966>)
 *  \date 11 07 2019
 */

#include "BilateralFilter.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

namespace ipcv {

/** Bilateral filter an image
 *
 *  \param[in] src             source cv::Mat of CV_8UC3
 *  \param[out] dst            destination cv::Mat of ddepth type
 *  \param[in] sigma_distance  standard deviation of distance/closeness filter
 *  \param[in] sigma_range     standard deviation of range/similarity filter
 *  \param[in] radius          radius of the bilateral filter (if negative, use
 *                             twice the standard deviation of the distance/
 *                             closeness filter)
 *  \param[in] border_mode     pixel extrapolation method
 *  \param[in] border_value    value to use for constant border mode
 */
bool BilateralFilter(const cv::Mat& src, cv::Mat& dst,
                     const double sigma_distance, const double sigma_range,
                     int radius, const BorderMode border_mode,
                     uint8_t border_value) {

  //initialize variables
  double temp_c;
  double temp_s;
  double bilat_sum;
  double bilat_norm;
  int kernel_size;
  double temp_x;
  if (radius < 0){
	  radius = 2*sigma_distance;
  }
  kernel_size = 2*radius+1;
  
  //initialize mats
  cv::Mat src_border;
  vector<cv::Mat> dst_lab;
  cv::Mat src_lab, src_l, dst_l;
  src_lab.create(src_border.rows, src_border.cols, CV_8UC3);
  src_l.create(src_border.rows, src_border.cols, CV_8UC1);
  dst_l.create(src.rows, src.cols, CV_8UC1);
  cv::Mat kernel_c;
  kernel_c.create (kernel_size, kernel_size, CV_32F);
  vector<cv::Mat> split_lab;
  
  //add border
  int bm = 1;
  if (border_mode == ipcv::BorderMode::CONSTANT) bm=0;
  copyMakeBorder (src, src_border, radius, radius, radius, radius, bm, border_value);

  //convert to lab and split image
  cv::cvtColor(src, dst, cv::COLOR_BGR2Lab);
  cv::split(dst, dst_lab);
  cv::cvtColor(src_border, src_lab, cv::COLOR_BGR2Lab);
  cv::split(src_lab, split_lab);
  src_l = split_lab[0];
  

  //create closeness function
  for (int rc=0; rc<kernel_c.rows; rc++) {
	  for (int cc=0; cc<kernel_c.cols; cc++) {
		  temp_c = sqrt(((rc-radius)*(rc-radius))+((cc-radius)*(cc-radius)));
		  temp_c = temp_c/sigma_distance;
		  temp_c = -0.5*(temp_c*temp_c);
		  temp_c = exp(temp_c);
		  kernel_c.at<float>(rc,cc) = temp_c;
	  }
  }

  //loop through image
  for (int r = 0; r < src_border.rows-kernel_size; r++) {
	  for (int c = 0; c < src_border.cols-kernel_size; c++) {
		  temp_s = 0;
		  bilat_sum = 0;
		  bilat_norm = 0;
		  //create similarity filter
		  for (int rs = 0; rs<kernel_c.rows; rs++) {
			  for (int cs=0; cs<kernel_c.cols; cs++) {
				  temp_s = src_l.at<uchar>(r+rs, c+cs)-src_l.at<uchar>(r+radius,c+radius);
				  temp_s = (temp_s*temp_s)*2.0;
				  temp_s = sqrt(temp_s);
				  temp_s = temp_s/sigma_range;
				  temp_s = -0.5*(temp_s*temp_s);
				  temp_s = exp(temp_s);
				  bilat_sum += temp_s*kernel_c.at<float>(rs,cs)*src_l.at<uchar>(r+rs,c+cs);
				  bilat_norm += temp_s*kernel_c.at<float>(rs,cs);
			  }
		  }
		  dst_l.at<uchar>(r,c) = static_cast<uchar>(bilat_sum/bilat_norm);
	  }
  }

  //put values in dst
  dst_lab[0] = dst_l;
  cv::merge(dst_lab, dst);
  cv::cvtColor(dst, dst, cv::COLOR_Lab2BGR);

  return true;
}
}

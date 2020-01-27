/*Implementation file for character recognition
 *
 * file ipcv/character_recognition/character_recognition.cpp
 * author <Allison Hazebrouck> (<amh7966>)
 * date 10 31 2019
 */


#include "CharacterRecognition.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

namespace ipcv {

bool character_recognition(const cv::Mat& src, const std::vector<cv::Mat>& filter, std::vector<int>& results, const std::string& method) {
	

	//vector
	if (method == "vector") {
		//variables
		vector <double> kernel_vec;
		vector <double> src_vec_temp;
		double cos_theta;
		float thresh = 0.97;
		double dot_product;
		vector <vector<double>> src_vec((src.rows-16)*(src.cols-21));
		int src_pos =0;
		cv::Mat src_crop;

		//get all possible segments of src image
		for (int r = 0; r < src.rows-22; r++) {
			for (int c = 0; c < src.cols-17; c++) {
				src_crop.release();
				src_crop = src(cv::Rect(c,r,17,22));
				src_vec_temp.clear();
				for (int rc = 0; rc<src_crop.rows; rc++) {
					for (int cc = 0; cc<src_crop.cols; cc++) {
						src_vec_temp.push_back(src_crop.at<float>(rc,cc));
						src_vec[src_pos] = src_vec_temp;
					}
				}
				src_pos++;
			}
		}
		
		//loop through kernels		
		for (int kern_num = 0; kern_num < 40; kern_num++) {
			kernel_vec.clear();	
			for (int r = 0; r<filter[kern_num].rows; r++) {
				for (int c = 0; c<filter[kern_num].cols; c++) {
					kernel_vec.push_back(filter[kern_num].at<float>(r,c));
				}
			}

			for (src_pos = 0; src_pos < ((src.rows-16)*(src.cols-21)); src_pos++) {
				src_vec_temp.clear();
				src_vec_temp = src_vec[src_pos];
				dot_product = 0;
				for (int pos = 0; pos < src_vec_temp.size(); pos++) {
					dot_product += ((src_vec_temp[pos])*(kernel_vec[pos]));
				}
				cos_theta = dot_product/(sqrt(cv::sum(src_vec_temp)[0])*sqrt(cv::sum(kernel_vec)[0]));
				if (cos_theta > thresh) {
					results[kern_num] += 1;
				}
			}
		}
	}



	//spatial 
	else if (method == "spatial") {
		//variables
		vector<cv::Mat> dst;
		float thresh = 0.99999;
		int ddepth = -1;
		cv::Mat dst_temp;
	
		//loop through kernels/filters
		for (int kern_num = 0; kern_num < 40; kern_num++) {
	
			dst_temp.release();
			cv::Mat filter_temp = filter[kern_num];
			double weight = cv::sum(filter_temp)[0];
			filter_temp = filter_temp/weight;

			cv::filter2D(src, dst_temp, ddepth, filter_temp);
			dst_temp.convertTo(dst_temp, CV_32FC1);

			//check for match and track count
			for (int r = 0; r<dst_temp.rows; r++) {
				for (int c = 0; c<dst_temp.cols; c++) {
					if (dst_temp.at<float>(r,c) >= thresh) {
						results[kern_num] += 1;
					}
				}
			}
			dst.push_back(dst_temp);
		}
   /*
   //print text
		vector <int> letter_pos;

		if (print == "print") {
			for (int r = 0; r < dst_temp.rows; r++) {
				for (int c = 0; c < dst_temp.cols; c++) {
					for (int kern_num = 0; kern_num < 40; kern_num++) {
						//
						dst_temp.release();
						dst_temp = dst[kern_num];
						if (dst_temp.at<float>(r,c) > thresh) {
							letter_pos.push_back(kern_num);
						}
					}
				}
			}

		}
   */
	}


	else {
		cout << "invalid method type" << endl;
		return false;
	}

	return true;



}
}


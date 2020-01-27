/** Interface file for Character Recognition
 *
 *  \file ipcv/character_recognition/character_recognition.h
 *  \author <Allison Hazebrouck> (<amh7966>)
 *  \date 10 31 2019
 */

#pragma once

#include <opencv2/core.hpp>

namespace ipcv {


bool character_recognition(const cv::Mat& src, const std::vector<cv::Mat>& filter, std::vector<int>&results, const std::string& method);
		
}


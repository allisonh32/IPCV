/** Implementation file for finding Otsu's threshold
 *
 *  \file ipcv/otsus_threshold/OtsusThreshold.cpp
 *  \author Allison Hazebrouck (amh7966)
 *  \date 09 19 2019
 */

#include "OtsusThreshold.h"

#include <iostream>

#include <cmath>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Find Otsu's threshold for each channel of a 3-channel (color) image
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[out] threshold   threshold values for each channel of a 3-channel
 *                          color image in cv::Vec3b
 */
bool OtsusThreshold(const cv::Mat& src, cv::Vec3b& threshold) {
  // create histogram, pdf, and cdf from src
  cv::Mat_<int> h;
  ipcv::Histogram(src, h);

  cv::Mat_<double> pdf;
  ipcv::HistogramToPdf(h, pdf);

  cv::Mat_<double> cdf;
  ipcv::HistogramToCdf(h, cdf);

  // iterate through channels
  for (int channel = 0; channel < 3; channel++) {
    int thresh_guess;         // k in paper
    vector<double> goodness;  // omega squared b of k in paper
    double goodness_temp;

    // get vector of mu k for code values between k=0 and 255
    vector<double> mu_k;
    double mu_k_temp = 0;
    for (int k = 0; k < 256; k++) {
      mu_k_temp += k * pdf.at<double>(channel, k);
      mu_k.push_back(mu_k_temp);
    }

    // fill goodness vector
    for (thresh_guess = 0; thresh_guess < 256; thresh_guess++) {
      // get value of goodness (omega ^2 b [k]) at thresh_guess (k)
      goodness_temp = (mu_k[255] * cdf.at<double>(channel, thresh_guess)) -
                      mu_k[thresh_guess];
      goodness_temp = pow(goodness_temp, 2);
      goodness_temp /= cdf.at<double>(channel, thresh_guess) *
                       (1 - cdf.at<double>(channel, thresh_guess));
      // add to vector
      goodness.push_back(goodness_temp);
      if (goodness[thresh_guess] != goodness[thresh_guess]) {
        goodness[thresh_guess] = 0;
      }
    }

    // get max goodness value and put in threshold Vec3b
    threshold(channel) =
        max_element(goodness.begin(), goodness.end()) - goodness.begin();
  }

  return true;
}
}

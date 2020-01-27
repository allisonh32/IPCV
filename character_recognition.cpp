#include <ctime>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>
#include <algorithm>
#include <string>

#include "imgs/ipcv/character_recognition/CharacterRecognition.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  bool verbose = false;
  string src_filename = "";
  string path = "";
  string method = "";

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
    "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
    "source-filename,i", po::value<string>(&src_filename), "source filename")(
    "filter-location,k", po::value<string>(&path), "path to filter images")(
    "method,m", po::value<string>(&method), "method of character recognition (spatial or vector)");

  po::positional_options_description positional_options;
  positional_options.add("source-filename", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
		  .options(options)
		  .positional(positional_options)
		  .run(),
		  vm);
  po::notify(vm);

  if (vm.count("help")) {
	  cout << "Usage: " << argv[0] << " [options] source-filename" << endl;
	  cout << options << endl;
	  return 0;
  }

  if (!boost::filesystem::exists(src_filename)) {
	  cerr << "Provided source file does not exist" << endl;
	  return 1;
  }

  //get sourcce file
  cv::Mat src = cv::imread(src_filename, cv::IMREAD_GRAYSCALE);
  src.convertTo(src, CV_32FC1);
  src = (255-src)/255;

  //get kernels
  cv::Mat filter_temp;
  vector<cv::Mat> filter;

  string extension = ".tif";
  const auto& entries = std::filesystem::directory_iterator(path);

  vector<string> files;
  for (const auto& entry : entries) {
	  if (entry.path().extension() == extension) {
		  files.push_back(entry.path());
	  }
  }
  std::sort(files.begin(), files.end());

  for (const auto& file : files) {
	  filter_temp=cv::imread(file, cv::IMREAD_GRAYSCALE);
	  filter_temp.convertTo(filter_temp, CV_32FC1);
	  filter.push_back((255-filter_temp)/255);
  }

  vector <int> results(40,0);
  vector <char> characters {
	  '\'', ',', '-', '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

  clock_t startTime = clock();
  
  ipcv::character_recognition(src, filter, results, method);

  for (int i=0;i<40;i++) {
	  cout << characters[i] << ": " << results[i] << endl;
  }

  clock_t endTime = clock();
  
  cv::imshow("source file", src);
  cv::waitKey(0);

  if (verbose) {
	  cout << "Source filename: " << src_filename << endl;
    cout << "Method: " << method << endl;
	  cout << "Size: " << src.size() << endl;
	  cout << "Elapsed time: " << (endTime - startTime) / static_cast<double>(CLOCKS_PER_SEC)
		  << "[s]" << endl;
  }

  return 0;
}

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <string>

// keep in mind that all or some of these includes
// may change depending on the library installation and version
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


int main(int argc, char * argv[]) {

    // declare image containers 
    std::vector<std::string> images_name;
    int i = 1;
    while(!is_number(argv[i])){
        images_name.push_back(argv[i]);
        i++;
    }

    std::vector<cv::Mat> img(images_name.size()),dst(images_name.size());

    // read one image from disk from disk
    auto start = std::chrono::high_resolution_clock::now();
    img = cv::imread(images_name[0]);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"read took: "<< usec << std::endl;

    // apply first filter, producing a new image (just for example)
    start = std::chrono::high_resolution_clock::now();
    cv::GaussianBlur(img, dst, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"Gaussian filter took: "<< usec << std::endl;

    // apply second filter. Here we overwrite the input (just for example)
    start = std::chrono::high_resolution_clock::now();
    cv::Sobel(dst,dst,-1, 1, 1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"Sobel filter took: "<< usec << std::endl;

    // write image to disk
    start = std::chrono::high_resolution_clock::now();
    cv::imwrite("img/out"+images_name[0],dst);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"write took: "<< usec << std::endl;

    return(0);
}
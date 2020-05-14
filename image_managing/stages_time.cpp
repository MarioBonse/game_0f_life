#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <string>
#include <dirent.h>

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

using namespace cv;

int main(int argc, char * argv[]) {

    // declare image containers 
    std::vector<std::string> images_name;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    int i = 1;
    std::string path = argv[1];
    if(path[path.length() - 1] != '/')
        path = path + '/';
    int n = path.length(); 
    char path_char[n + 1]; 
  
    strcpy(path_char, path.c_str()); 
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path_char) ) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
        std::string ret(ent->d_name);
        if(ret =="." || ret == "..")
            continue;
        images_name.push_back(ret);
    }
    closedir (dir);
    } else {
    /* could not open directory */
    perror ("");
    return EXIT_FAILURE;
    }

    std::vector<cv::Mat> imgages,dst;
    cv::Mat src, result;

    // read one image from disk from disk
    auto start = std::chrono::high_resolution_clock::now();
    for(auto img: images_name)
        imgages.push_back(cv::imread(path + images_name[1]));   // Read the file
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"read took: "<< usec << std::endl;

    // apply first filter, producing a new image (just for example)
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i<images_name.size();i++)
        cv::GaussianBlur(imgages[i], imgages[i], cv::Size(3, 3), 0, 0, BORDER_DEFAULT);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"Gaussian filter took: "<< usec << std::endl;

    // apply second filter. Here we overwrite the input (just for example)
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i<images_name.size();i++)
        cv::Sobel( imgages[i],imgages[i], ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"Sobel filter took: "<< usec << std::endl;

    // write image to disk
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i<images_name.size();i++){
        //std::string name = "img/out" + images_name[i];
        //std::cout<<name<<std::endl;
        cv::imwrite("img/out" + images_name[i], imgages[i]);
    }
    elapsed = std::chrono::high_resolution_clock::now() - start;
    usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout <<"write took: "<< usec << std::endl;

    return(0);
}
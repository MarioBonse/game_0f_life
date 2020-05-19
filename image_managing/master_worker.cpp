#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <string>
#include <dirent.h>
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int main(int argc, char * argv[]) {

    if(argc < 3){
        std::cerr << "use: " << argv[0]  << " source dest [iterations] [nw] \n";
        return -1;
    }
    // declare image containers 
    std::vector<std::string> images_name;
    int interations = 1;
    int nw = 1;
    if(argc>4)
        nw = atoi(argv[4]);
    if(argc > 3)
        interations = atoi(argv[3]);
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    int i = 1;
    std::string path = argv[1];
    std::string dest = argv[2];

    if(dest[dest.length() - 1] != '/')
        dest = dest + '/';

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

    int number_images = images_name.size();
    std::vector<cv::Mat> images(number_images*interations);
    cv::Mat src, result;
    std::vector<std::string> images_name_repeated(number_images*interations);
    // read one image from disk from disk
    auto start = std::chrono::high_resolution_clock::now();

    for(int j = 0; j<interations; j++){
        for(int i = 0; i<number_images; i++)
            images_name_repeated[j*number_images + i] = images_name[i];
    }
    #pragma omp parallel for num_threads(nw) schedule(dynamic)
    for(i = 0; i<images_name_repeated.size(); i++){
            std::string read = path + images_name_repeated[i];
            
            images[i] = (cv::imread(read));   // Read the file

            cv::GaussianBlur(images[i], images[i], cv::Size(3, 3), 0, 0, BORDER_DEFAULT);

            cv::Sobel( images[i],images[i], ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
            //write the image
            std::string res = dest + std::to_string(i/images_name_repeated.size()) + "out" + images_name_repeated[i];
            cv::imwrite(res, images[i]);
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout << nw <<", "<< usec << std::endl;

    return(0);
}
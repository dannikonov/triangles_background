#ifndef TRIANGULATE_EXAMPLE_H
#define TRIANGULATE_EXAMPLE_H

#include "opencv4/opencv2/core.hpp"
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include <thread>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

class Example {
private:
    std::string _filename;
    cv::Size _size;
    cv::Mat _img;
    cv::Mat _mask;

public:
    Example(std::string filename);

    void bw_with_mask();

    void mask_use_alpha();

    void show();

    void save(const std::string &filename);

};


#endif //TRIANGULATE_EXAMPLE_H

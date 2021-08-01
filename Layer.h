#ifndef TRIANGULATE_LAYER_H
#define TRIANGULATE_LAYER_H

#include "opencv4/opencv2/core.hpp"
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include <iostream>
#include <map>

#include "Filter.h"

using std::cout;
using std::endl;

typedef cv::Point TRIANGLE[3];

class Layer {
private:
    cv::Mat mask;
    FILTER filter;
    std::mutex mtx;

public:
    Layer(cv::Size size, FILTER filter);

    ~Layer();

    cv::Mat &get_mask();

    FILTER get_filter();

    void add_triangle(TRIANGLE points);

    void add_solid_triangle(TRIANGLE points);

    void add_gradient_triangle(TRIANGLE points);

    // @todo mask2
    void gradient_section(cv::Mat &mask2, TRIANGLE points);
};

#endif //TRIANGULATE_LAYER_H

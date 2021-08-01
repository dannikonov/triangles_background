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

class Layer {
private:
    cv::Mat mask;
    FILTER filter;

public:
    Layer(cv::Size size, FILTER filter);

    cv::Mat &get_mask();

    FILTER get_filter();

    ~Layer();
};

#endif //TRIANGULATE_LAYER_H

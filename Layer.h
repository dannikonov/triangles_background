#ifndef TRIANGULATE_LAYER_H
#define TRIANGULATE_LAYER_H

#include "opencv4/opencv2/core.hpp"
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include <iostream>
#include <map>

using std::cout;
using std::endl;

#define FILTER_BLUR 1
#define FILTER_FILL 2

#define FILTER_INC_SATURATE 3
#define FILTER_DEC_SATURATE 4

#define FILTER_INC_LIGHTNESS 5
#define FILTER_DEC_LIGHTNESS 6

#define FILTER_BW 7
#define FILTER_COLORMAP 8

#define FILTER_NOTHING 9999

typedef void (*FILTER)(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

class Layer {
private:
    cv::Mat _mask;
    int _filter;
    std::map<int, FILTER> _filters;

public:
    Layer(cv::Size size, int filter);

    cv::Mat &get_mask();

    FILTER get_filter();

    ~Layer();
};

#endif //TRIANGULATE_LAYER_H

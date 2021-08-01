#include "Layer.h"

// public
Layer::Layer(cv::Size size, FILTER filter) :
        mask(cv::Mat(size, CV_8U, cv::Scalar(0))),
        filter(filter) {
    std::srand(std::time(nullptr));
}

cv::Mat &Layer::get_mask() {
    return mask;
}

FILTER Layer::get_filter() {
    return filter;
}

Layer::~Layer() {
}
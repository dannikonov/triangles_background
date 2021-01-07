#include "Layer.h"

// public
Layer::Layer(cv::Size size, int filter) :
        _mask(cv::Mat(size, CV_8U, cv::Scalar(0))),
        _filter(filter),
        _filters() {
    std::srand(std::time(nullptr));

    FILTER nothing = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        *output = *input;
    };

    FILTER blur = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        cv::blur(*input, *output, cv::Size(10, 10));
    };

    FILTER fill = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        cv::Mat tmp;
        cv::cvtColor(*mask, tmp, cv::COLOR_RGB2GRAY);
        output->setTo(cv::mean(*input, tmp));
    };

    FILTER bw = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        cv::Mat tmp;

        cv::cvtColor(*input, tmp, cv::COLOR_RGB2GRAY);
        cv::cvtColor(tmp, tmp, cv::COLOR_GRAY2RGB);
        tmp.convertTo(tmp, CV_8UC3);

        tmp.copyTo(*output, *mask);
    };

    FILTER inc_saturate = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        input->convertTo(*output, CV_8UC3, 1, 15);
    };

    FILTER dec_saturate = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        input->convertTo(*output, CV_8UC3, 1, -15);
    };

    FILTER inc_lightness = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        input->convertTo(*output, CV_8UC3, 1.1, 0);
    };

    FILTER dec_lightness = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        input->convertTo(*output, CV_8UC3, 0.9, 0);
    };

    FILTER colorMap = [](cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        applyColorMap(*input, *output, cv::COLORMAP_BONE);
    };

    _filters.emplace(std::make_pair(FILTER_NOTHING, nothing));
    _filters.emplace(std::make_pair(FILTER_BLUR, blur));
    _filters.emplace(std::make_pair(FILTER_FILL, fill));
    _filters.emplace(std::make_pair(FILTER_BW, bw));
    _filters.emplace(std::make_pair(FILTER_INC_SATURATE, inc_saturate));
    _filters.emplace(std::make_pair(FILTER_DEC_SATURATE, dec_saturate));
    _filters.emplace(std::make_pair(FILTER_INC_LIGHTNESS, inc_lightness));
    _filters.emplace(std::make_pair(FILTER_DEC_LIGHTNESS, dec_lightness));
    _filters.emplace(std::make_pair(FILTER_COLORMAP, colorMap));
}

cv::Mat &Layer::get_mask() {
    return _mask;
}

FILTER Layer::get_filter() {
    return _filters.at(_filter);
}

Layer::~Layer() {
}
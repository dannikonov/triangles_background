#include "Layer.h"

// public
Layer::Layer(cv::Size size, FILTER filter) :
        layer_mask(cv::Mat(size, CV_8U, cv::Scalar(0))),
        filter(filter) {
    std::srand(std::time(nullptr));
}

Layer::~Layer() {
}

cv::Mat &Layer::get_mask() {
    return layer_mask;
}

FILTER Layer::get_filter() {
    return filter;
}

void Layer::add_triangle(TRIANGLE points) {
//    if (static_cast<int>(rand()) % 3 == 0) {
//        add_solid_triangle(layer_mask, points);
//    } else {
    add_gradient_triangle(layer_mask, points);
//    }
}

void Layer::add_solid_triangle(cv::Mat target, TRIANGLE points) {
//    cout << points[0] << points[1] << points[2] << endl;

    mtx.lock();
    fillConvexPoly(target, points, 3, cv::Scalar(255), 8);
    mtx.unlock();
}

// @todo mask2
void Layer::add_gradient_triangle(cv::Mat target, TRIANGLE points) {
    cv::Mat mask = cv::Mat(target.size(), target.type(), cv::Scalar(0));
    add_solid_triangle(mask, points);

    cv::Mat gradient = cv::Mat(target.size(), target.type(), cv::Scalar(0));
    gradient_section(gradient, points);

    mtx.lock();
    gradient.copyTo(target, mask);
    mtx.unlock();
}

// @todo mask
void Layer::gradient_section(cv::Mat &mask, TRIANGLE points) {
    std::sort(points, points + 3, [](cv::Point a, cv::Point b) { return a.y < b.y; });

    int ymin = points[0].y;
    int ymax = points[2].y;

    int rows = ymax - ymin + 1;

    bool gradient_direction_inc = static_cast<int>(rand()) % 2;

    int color_start = 0;
    int color_end = 255;
    if (!gradient_direction_inc) {
        std::swap(color_start, color_end);
    }

    int color;
    double delta = static_cast<double>(color_end - color_start) / (rows - 1);

    for (int r = ymin, i = 1; r < ymax; r++, i++) {
        color = static_cast<int> (round(color_start + delta * i));
        mask.row(r).setTo(cv::Scalar(color));
    }
}


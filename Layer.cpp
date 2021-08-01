#include "Layer.h"

// public
Layer::Layer(cv::Size size, FILTER filter) :
        mask(cv::Mat(size, CV_8U, cv::Scalar(0))),
        filter(filter) {
    std::srand(std::time(nullptr));
}

Layer::~Layer() {
}

cv::Mat &Layer::get_mask() {
    return mask;
}

FILTER Layer::get_filter() {
    return filter;
}

void Layer::add_triangle(TRIANGLE points) {
    mtx.lock();
    if (static_cast<int>(rand()) % 3 == 0) {
        add_solid_triangle(points);
    } else {
        add_gradient_triangle(points);
    }
    mtx.unlock();
}

void Layer::add_solid_triangle(TRIANGLE points) {
    fillConvexPoly(mask, points, 3, cv::Scalar(255), 8);
}

// @todo mask2
void Layer::add_gradient_triangle(TRIANGLE points) {
    cv::Mat mask2 = cv::Mat(mask.size(), mask.type(), cv::Scalar(0));
    add_solid_triangle(points);

    cv::Mat gradient = cv::Mat(mask.size(), mask.type(), cv::Scalar(0));
    gradient_section(gradient, points);

    gradient.copyTo(mask, mask2);
}

// @todo mask2
void Layer::gradient_section(cv::Mat &mask2, TRIANGLE points) {
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
        mask2.row(r).setTo(cv::Scalar(color));
    }
}


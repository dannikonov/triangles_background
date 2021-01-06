#include "Example.h"

// private

// public
Example::Example(std::string filename)
        : _filename(std::move(filename)) {
    int r = 600;
    int c = 600;
    _size = cv::Size_(r, c);
    _img = cv::Mat::zeros(_size, CV_8UC3);

    // init colors
    cv::Scalar red = cv::Scalar(0, 0, 255);
    cv::Scalar green = cv::Scalar(0, 255, 0);

    // init area
    cv::rectangle(_img, cv::Rect(0.1 * c, 0.1 * r, 0.8 * c, 0.8 * r), green, -1);
    cv::circle(_img, cv::Point(0.5 * c, 0.5 * r),  r / 5, red, -1);

    // init mask
    _mask = cv::Mat(_size, CV_8U, cv::Scalar(0));
    cv::Point triangle[3] = {
            cv::Point(_size.width, _size.height),
            cv::Point(_size.width, 0),
            cv::Point(0, _size.height),
    };

    add_gradient_triangle(_mask, triangle);
}

void Example::add_triangle(cv::Mat &target, cv::Point *points) {
    cv::fillConvexPoly(target, points, 3, cv::Scalar(255));
}

void Example::gradient_section(cv::Mat &target, cv::Point *points) {
    std::sort(points, points + 3, [](cv::Point a, cv::Point b) { return a.y < b.y; });

    int ymin = points[0].y;
    int ymax = points[2].y;

    int rows = ymax - ymin + 1;

    int color_start = 0;
    int color_end = 255;
    // @todo support gradient direction

    int color;
    for (int r = ymin, i = 0; r < ymax; r++, i++) {
        color = static_cast<int> (round((double) ((color_end - color_start) * i) / rows));
        target.row(r).setTo(cv::Scalar(color));
    }
}

void Example::bw_with_mask() {
    cv::Mat bw;
    cv::cvtColor(_img, bw, cv::COLOR_BGR2GRAY);
    cv::cvtColor(bw, bw, cv::COLOR_GRAY2BGR);
    bw.convertTo(bw, CV_8UC3);

    cv::Mat result;

    for (int r = 0; r < _img.rows; r++) {
        cv::Rect row = cv::Rect(0, r, _img.cols, 1);
        cv::Mat roi_i(_img, row);
        cv::Mat roi_f(bw, row);

        cv::Mat row_mask(_mask, row);
        cv::Mat roi_r;;

        cv::Scalar c = cv::mean(row_mask);
        double alpha = c[0] / 255;

        cv::addWeighted(roi_i, alpha, roi_f, 1 - alpha, 0, roi_r);

        result.push_back(roi_r);
    }

    result.copyTo(_img, _mask);
}

void Example::mask_use_alpha() {
    cv::Mat filter;
//    _img.convertTo(filter, CV_8UC3);
    cv::cvtColor(_img, filter, cv::COLOR_BGR2RGB);
    cv::cvtColor(filter, filter, cv::COLOR_RGB2BGR);
    filter.convertTo(filter, CV_8UC3, 0.9, 2);

    cv::Mat Result = (_img & (~_mask)) + filter;

//    Result = Result.mul(_mask);
// Result += filter;

    Result.copyTo(_img, _mask);

    cv::Mat img_bgra;
    cv::cvtColor(_img, img_bgra, cv::COLOR_BGR2BGRA);
}

void Example::add_gradient_triangle(cv::Mat &layer, cv::Point *triangle) {
    cv::Mat mask(layer.size(), layer.type());
    add_triangle(mask, triangle);

    cv::Mat gradient(layer.size(), layer.type());
    gradient_section(gradient, triangle);

    gradient.copyTo(layer, mask);
}

void Example::show() {
    imshow("Image", _img);
    cv::waitKey();
}

void Example::save(const std::string &filename) {
    imwrite(filename, _img);
}
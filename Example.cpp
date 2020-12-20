#include "Example.h"

// private

// public
Example::Example(std::string filename)
        : _filename(std::move(filename)) {
    _size = cv::Size_(400, 400);
    _img = cv::Mat::zeros(_size, CV_8UC3);

    // init colors
    cv::Scalar red = cv::Scalar(0, 0, 255);
    cv::Scalar green = cv::Scalar(0, 255, 0);

    // init area
    cv::rectangle(_img, cv::Rect(50, 50, 300, 300), green, -1);
    cv::circle(_img, cv::Point(200, 200), 100, red, -1);

    // init mask
    _mask = cv::Mat::zeros(_size, CV_8UC3);
    cv::Point triangle[3] = {
            cv::Point(_size.width, 0),
            cv::Point(_size.width, _size.height),
            cv::Point(0, _size.height),
    };
    cv::fillConvexPoly(_mask, triangle, 3, cv::Scalar(255, 255, 255));
}

void Example::bw_with_mask() {
    cv::Mat bw;
    cv::cvtColor(_img, bw, cv::COLOR_RGB2GRAY);
    cv::cvtColor(bw, bw, cv::COLOR_GRAY2RGB);
    bw.convertTo(bw, CV_8UC3);

    cv::Mat result;

    for (int r = 0; r < _img.rows; r++) {
        cv::Rect row = cv::Rect(0, r, _img.cols, 1);
        cv::Mat roi_i(_img, row);
        cv::Mat roi_f(bw, row);

        cv::Mat row_mask(_mask, row);
        cv::Mat roi_r = roi_f & row_mask;

        double alpha = 0.5;
        cv::addWeighted(roi_i, alpha, roi_r, 1 - alpha, 0, roi_r);
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

void Example::show() {
    imshow("Image", _img);
    cv::waitKey();
}

void Example::save(const std::string &filename) {
    imwrite(filename, _img);
}
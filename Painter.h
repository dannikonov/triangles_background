#ifndef TRIANGULATE_PAINTER_H
#define TRIANGULATE_PAINTER_H

#define SQRT3 1.73205080756887729352

#include <opencv2/opencv.hpp>
//#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>

//struct img {
//    int cols;
//    int rows;
//    int h;
//    int w;
//};

class Painter {
private:
    typedef void (Painter::*CALLBACK)(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    std::string _filename;
    int _a;
    double _h;
    cv::Mat _img;
    cv::Size _size;
    int _cols;
    int _rows;

    std::vector<std::vector<cv::Point>> _points;
    std::vector<double> _scale;

    void _calculate_scale();

    void _calculate_points();

    int _callback();

    void _drawTriangle(cv::Point *points, cv::Mat *m);

    void _draw();

    // callbacks
    std::vector<CALLBACK> _callbacks;

    void _blur(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _fill(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask);



public:
    Painter(std::string filename, int a);

    void show();

    void save(std::string path);
};


#endif //TRIANGULATE_PAINTER_H

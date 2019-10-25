#ifndef TRIANGULATE_PAINTER_H
#define TRIANGULATE_PAINTER_H

#define SQRT3 1.73205080756887729352

#include <opencv2/opencv.hpp>
//#include <opencv4/opencv2/opencv.hpp>
//#include <boost/thread/thread.hpp>
#include <thread>
#include <iostream>
#include <iomanip>

//struct img {
//    int cols;
//    int rows;
//    int h;
//    int w;
//};

using std::cout;
using std::endl;

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
    std::vector<cv::Mat> _layers;

    void _calculate_scale();

    void _calculate_points();

    int _callback();

    void _addTriangle(cv::Point *points);

    void _drawLayer(int index);

    void _draw();

    // callbacks
    std::vector<CALLBACK> _callbacks;

    void _blur(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _fill(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _bw(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _inc_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _dec_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _inc_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _dec_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

public:
    Painter(std::string filename, int a);

    void show();

    void save(std::string path);
};


#endif //TRIANGULATE_PAINTER_H

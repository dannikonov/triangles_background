#ifndef TRIANGULATE_PAINTER_H
#define TRIANGULATE_PAINTER_H

#define SQRT3 1.73205080756887729352

#include "opencv4/opencv2/core.hpp"
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include <thread>
#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::endl;

class Painter {
private:
    typedef void (Painter::*CALLBACK)(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    typedef std::pair<CALLBACK, double> CALLBACK_PAIR;

    typedef std::pair<int, int> COORDS;

    typedef cv::Point TRIANGLE[3];

    std::string _filename;
    int _step;
    double _a;
    double _h;
    cv::Mat _img;
    cv::Size _size;
    int _cols;
    int _rows;

    std::map<std::pair<int, int>, cv::Point> _points;

    std::vector<cv::Point*> _triangles;

    std::vector<cv::Mat> _layers;

    void _calculate_scale();

    void _calculate_test_triangles();

    void _calculate_triangles();

    void _calculate_small_triangle();

    cv::Point _point_by_coord(int x, int y);

    cv::Mat &_get_random_layer();

    void _add_triangle(TRIANGLE);

    void _drawLayer(int index);

//    void _gradient_mask(cv::Mat **input);

    // callbacks
    std::vector<CALLBACK_PAIR> _callbacks;

    void _blur(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _fill(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _bw(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _inc_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _dec_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _inc_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _dec_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _colorMap(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

    void _nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

public:
    Painter(std::string filename, int step, int a);

    void draw();

    void show();

    void save(const std::string &path);
};


#endif //TRIANGULATE_PAINTER_H

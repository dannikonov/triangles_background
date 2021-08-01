#ifndef TRIANGULATE_PAINTER_H
#define TRIANGULATE_PAINTER_H

#define SQRT3 1.73205080756887729352

#include "Layer.h"
#include "opencv4/opencv2/core.hpp"
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include <thread>
#include <iostream>
#include <iomanip>
#include <map>
#include <ctime>
#include "Filter.h"

using std::cout;
using std::endl;

typedef std::pair<int, int> COORDS;

typedef cv::Point TRIANGLE[3];

class Painter {
private:
    std::string _filename;
    int _step;
    double _a;
    double _h;
    cv::Mat _img;
    int _cols;
    int _rows;
    std::map<std::pair<int, int>, cv::Point> _points;
    std::map<int, Layer *> _layers;
//    std::map<std::string, double> _filters;
    std::map<int, double> _probability;
    std::map<int, FILTER> _filters;

    void _calculate_probability(std::map<int, double> &probability);

    void _calculate_test_triangles();

    void _calculate_triangles();

    void calculate_small_triangle();

    cv::Point point_by_coord(int x, int y);

    Layer *get_random_layer();

    void drawLayer(Layer *layer);

    void init_filters();

public:
    Painter(std::string filename, int step, int a);

    void draw();

    void show();

    void save(const std::string &path);

    ~Painter();
};

#endif //TRIANGULATE_PAINTER_H

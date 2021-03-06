#include "Painter.h"

#include <utility>

// private
void Painter::_calculate_probability() {
    // delete empty callbacks
//    for (auto it = _filters.begin(); it != _filters.end(); /* NOTHING */) {
//        if (it->second == 0) {
//            it = _filters.erase(it);
//        } else {
//            ++it;
//        }
//    }

    double sum = 0;
    for (auto &it : _filters) {
        sum += it.second;
        _probability[it.first] = sum;
    }

    // debug only
    int i = 0;
    for (auto &it : _probability) {
        cout << "filter[" << i++ << "]: " << it.second << endl;
    }
}

void Painter::_calculate_test_triangles() {
    int c = 0, r = 3;
    cv::Point p1[3] = {
            _point_by_coord(c, r),
            _point_by_coord(c + _step, r + _step),
            _point_by_coord(c - _step, r + _step)
    };

    _add_triangle(_get_random_layer(), p1);

    cv::Point p2[3] = {
            _point_by_coord(5, 5),
            _point_by_coord(7, 7),
            _point_by_coord(5, 7)
    };

    _add_triangle(_get_random_layer(), p2);
}

void Painter::_calculate_triangles() {
    for (int r = 0; r < _rows * _step; r += _step) {
        if ((r / _step) % 2) {
            for (int c = 0; c < _cols * 2 * _step + 1; c += 2 * _step) {
                TRIANGLE p1 = {
                        _point_by_coord(c, r),
                        _point_by_coord(c + _step, r + _step),
                        _point_by_coord(c - _step, r + _step)
                };

                _add_triangle(_get_random_layer(), p1);

                TRIANGLE p2 = {
                        _point_by_coord(c + _step, r + _step),
                        _point_by_coord(c + 2 * _step, r),
                        _point_by_coord(c, r)
                };

                _add_triangle(_get_random_layer(), p2);

            }
        } else {
            for (int c = 0; c < _cols * 2 * _step + 1; c += 2 * _step) {
                TRIANGLE p1 = {
                        _point_by_coord(c + _step, r),
                        _point_by_coord(c + 2 * _step, r + _step),
                        _point_by_coord(c, r + _step)
                };

                _add_triangle(_get_random_layer(), p1);

                TRIANGLE p2 = {
                        _point_by_coord(c, r + _step),
                        _point_by_coord(c - _step, r),
                        _point_by_coord(c + _step, r)
                };

                _add_triangle(_get_random_layer(), p2);
            }
        }
    }
}

void Painter::_calculate_small_triangle() {
    for (int r = 0; r < _rows * _step; r += _step) {
        for (int c = 0; c < _cols * 2 * _step + 1; c += _step) {
            int rnd = rand() % (_step + 1);

            if (0 < rnd && rnd < (_step / 2 + 1)) {
                TRIANGLE p1 = {
                        _point_by_coord(c, r),
                        _point_by_coord(c + rnd, r + rnd),
                        _point_by_coord(c - rnd, r + rnd)
                };

                _add_triangle(_get_random_layer(), p1);
            }
        }
    }
}

cv::Point Painter::_point_by_coord(int c, int r) {
    COORDS cr = std::make_pair(c, r);

    if (_points.find(cr) == _points.end()) {
        double x = c * _a / (2 * _step);
        double y = r * _h / _step;

        if (x < 0) x = 0;
        if (x >= _img.size().width) x = _img.size().width - 1;

        _points.insert({cr, cv::Point(x, y)});
    }

    return _points.at(cr);
}

cv::Mat &Painter::_get_random_layer() {
    double r_norm = (double) rand() / RAND_MAX;

    for (auto &it : _probability) {
        if (r_norm < it.second) {
            return _layers.at(it.first)->get_mask();
        }
    }

    return _layers.at(std::prev(_probability.end())->first)->get_mask();
}

void Painter::_add_triangle(cv::Mat &layer, TRIANGLE points) {
    if (static_cast<int>(rand()) % 5 == 0) {
        _add_solid_triangle(layer, points);
    } else {
        _add_gradient_triangle(layer, points);
    }
}

void Painter::_add_solid_triangle(cv::Mat &layer, cv::Point *points) {
    fillConvexPoly(layer, points, 3, cv::Scalar(255), 8);
}

void Painter::_add_gradient_triangle(cv::Mat &layer, TRIANGLE points) {
    cv::Mat mask = cv::Mat(layer.size(), layer.type(), cv::Scalar(0));
    _add_solid_triangle(mask, points);

    cv::Mat gradient = cv::Mat(layer.size(), layer.type(), cv::Scalar(0));
    _gradient_section(gradient, points);

    gradient.copyTo(layer, mask);
}

void Painter::_gradient_section(cv::Mat &mask, TRIANGLE points) {
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

void Painter::_drawLayer(Layer &layer) {
    cv::Mat img_f = cv::Mat::zeros(_img.size(), _img.type());

    FILTER f = layer.get_filter();
    cv::Mat layer_mask = layer.get_mask();

    f(&_img, &img_f, &layer_mask);

    cv::Mat result;
    for (int r = 0; r < _img.rows; r++) {
        cv::Rect row = cv::Rect(0, r, _img.cols, 1);
        cv::Mat roi_i(_img, row);
        cv::Mat roi_f(img_f, row);

        cv::Mat row_mask(layer_mask, row);
        cv::Mat roi_r(roi_i.size(), _img.type());

        for (int c = 0; c < roi_i.cols; c++) {
            double alpha = static_cast<double>(row_mask.at<uchar>(c)) / 255;

            for (int ch = 0; ch < roi_i.channels(); ch++) {
                roi_r.at<cv::Vec3b>(c)[ch] =
                        cv::saturate_cast<uchar>(
                                (1 - alpha) * roi_i.at<cv::Vec3b>(c)[ch]
                                +
                                (alpha) * roi_f.at<cv::Vec3b>(c)[ch]
                        );
            }
        }

        result.push_back(roi_r);
    }

    result.copyTo(_img, layer_mask);
}

// public
Painter::Painter(std::string filename, int step, int a) :
        _filename(std::move(filename)),
        _step(step),
        _a(a),
        _h(SQRT3 * _a / 2),
        _img(cv::imread(_filename, cv::IMREAD_COLOR)),
        _points(),
        _layers(),
        _probability(),
        _filters{
                {FILTER_BLUR,          0.2},
//                {FILTER_FILL, 0.08},

                {FILTER_INC_SATURATE,  0.2}, // +
                {FILTER_DEC_SATURATE,  0.2}, // +

                {FILTER_INC_LIGHTNESS, 0.1},
                {FILTER_DEC_LIGHTNESS, 0.1},
                {FILTER_BW,            0.1},
//                {FILTER_COLORMAP, 1.0 / 100},
                {FILTER_NOTHING,       1} // default
        } {
    std::srand(std::time(nullptr));

    int img_w = _img.size().width;
    int img_h = _img.size().height;

    _cols = (int) round(img_w / _a);
    _rows = (int) round(img_h / _h);

    int img_new_w = (int) round(_cols * _a);
    int img_new_h = (int) round(_rows * _h);

    cout << "w: " << img_new_w << " h: " << img_new_h << endl;
    cout << "cols: " << _cols << " rows: " << _rows << endl;
    cout << "a: " << _a << " h: " << _h << " step: " << _step << endl;
    cout << "h coef: " << (double) (img_h - img_new_h) * 100 / img_h << std::setprecision(5) << "%"
         << " w coef: " << (double) (img_w - img_new_w) * 100 / img_w << std::setprecision(5) << "%" << endl;

    resize(_img, _img, cv::Size(img_new_w, img_new_h));

    // init layers
    for (auto &it : _filters) {
        _layers.emplace(std::make_pair(it.first, new Layer(_img.size(), it.first)));
    }
}

void Painter::draw() {
    clock_t tStart;

    tStart = clock();
    _calculate_probability();
    cout << "_calculate_probability: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

//    _calculate_test_triangles();
    tStart = clock();
    // @todo optimize
    _calculate_triangles();
    cout << "_calculate_triangles: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

    tStart = clock();
    // @todo optimize
    _calculate_small_triangle();
    cout << "_calculate_small_triangle: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

    tStart = clock();
    std::vector<std::thread> t;
    for (auto &it: _layers) {
        _drawLayer(*it.second); // do disable multithreading
//        t.push_back(std::move(std::thread(&Painter::_drawLayer, this, std::ref(*it.second))));
    }

    for (std::thread &th : t) {
        if (th.joinable()) {
            th.join();
        }
    }
    cout << "_drawLayer: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;
}

void Painter::show() {
    imshow("Image", _img);
    cv::waitKey();
}

void Painter::save(const std::string &filename) {
    imwrite(filename, _img);
}

Painter::~Painter() {
    for (auto &it : _layers) {
        delete (it.second);
    }
}
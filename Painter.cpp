#include "Painter.h"

#include <utility>

// private
void Painter::_calculate_probability(std::map<int, double> &probability) {
    // delete empty callbacks
//    for (auto it = _filters.begin(); it != _filters.end(); /* NOTHING */) {
//        if (it->second == 0) {
//            it = _filters.erase(it);
//        } else {
//            ++it;
//        }
//    }

    double sum = 0;
    for (auto &it : probability) {
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
            point_by_coord(c, r),
            point_by_coord(c + _step, r + _step),
            point_by_coord(c - _step, r + _step)
    };
// @todo
//    add_triangle(get_random_layer(), p1);

    cv::Point p2[3] = {
            point_by_coord(5, 5),
            point_by_coord(7, 7),
            point_by_coord(5, 7)
    };

// @todo
//    add_triangle(get_random_layer(), p2);
}

void Painter::_calculate_triangles() {
    std::vector<std::thread> t;

    for (int r = 0; r < _rows * _step; r += _step) {
        if ((r / _step) % 2) {
            for (int c = 0; c < _cols * 2 * _step + 1; c += 2 * _step) {
                TRIANGLE p1 = {
                        point_by_coord(c, r),
                        point_by_coord(c + _step, r + _step),
                        point_by_coord(c - _step, r + _step)
                };

                get_random_layer()->add_triangle(p1);
//                t.push_back(std::move(std::thread(&Layer::add_triangle, get_random_layer(), p1)));

                TRIANGLE p2 = {
                        point_by_coord(c + _step, r + _step),
                        point_by_coord(c + 2 * _step, r),
                        point_by_coord(c, r)
                };

                get_random_layer()->add_triangle(p2);
//                t.push_back(std::move(std::thread(&Layer::add_triangle, get_random_layer(), p2)));
            }
        } else {
            for (int c = 0; c < _cols * 2 * _step + 1; c += 2 * _step) {
                TRIANGLE p1 = {
                        point_by_coord(c + _step, r),
                        point_by_coord(c + 2 * _step, r + _step),
                        point_by_coord(c, r + _step)
                };

                get_random_layer()->add_triangle(p1);
//                t.push_back(std::move(std::thread(&Layer::add_triangle, get_random_layer(), p1)));

                TRIANGLE p2 = {
                        point_by_coord(c, r + _step),
                        point_by_coord(c - _step, r),
                        point_by_coord(c + _step, r)
                };

                get_random_layer()->add_triangle(p2);
//                cout << " add " << p2[0] << p2[1] << p2[2] << endl;
//                t.push_back(std::move(std::thread(&Layer::add_triangle, get_random_layer(), p2)));
            }
        }

//        for (std::thread &th : t) {
//            if (th.joinable()) {
//                th.join();
//            }
//        }
    }

}

/**
 * @deprecated
 * works wrong!!
 */
void Painter::calculate_small_triangle() {
    std::vector<std::thread> t;
    for (int r = 0; r < _rows * _step; r += _step) {
        for (int c = 0; c < _cols * 2 * _step + 1; c += _step) {
            int rnd = rand() % (_step + 1);

            if (0 < rnd && rnd < (_step / 2 + 1)) {
                TRIANGLE p1 = {
                        point_by_coord(c, r),
                        point_by_coord(c + rnd, r + rnd),
                        point_by_coord(c - rnd, r + rnd)
                };

//                t.push_back(std::move(std::thread(&Layer::add_triangle, get_random_layer(), p1)));
                get_random_layer()->add_triangle(p1);
            }
        }

//        for (std::thread &th : t) {
//            if (th.joinable()) {
//                th.join();
//            }
//        }
    }
}

cv::Point Painter::point_by_coord(int c, int r) {
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

Layer *Painter::get_random_layer() {
    double r_norm = (double) rand() / RAND_MAX;

    for (auto &it : _probability) {
        if (r_norm < it.second) {
            return _layers.at(it.first);
        }
    }

    return _layers.at(std::prev(_probability.end())->first);
}


void Painter::drawLayer(Layer *layer, int t) {
    cout << "t:" << t << " " << sched_getcpu() << endl;
    cv::Mat img_f = cv::Mat::zeros(_img.size(), _img.type());

    FILTER f = layer->get_filter();
    cv::Mat layer_mask = layer->get_mask();

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

void Painter::init_filters() {


    _filters.emplace(std::make_pair(FILTER_NOTHING, Filter::get_filter(FILTER_NOTHING)));
    _filters.emplace(std::make_pair(FILTER_BLUR, Filter::get_filter(FILTER_BLUR)));
// @todo fill doesn't work
//    _filters.emplace(std::make_pair(FILTER_FILL, Filter::get_filter(FILTER_FILL)));
    _filters.emplace(std::make_pair(FILTER_BW, Filter::get_filter(FILTER_BW)));
    _filters.emplace(std::make_pair(FILTER_INC_SATURATE, Filter::get_filter(FILTER_INC_SATURATE)));
    _filters.emplace(std::make_pair(FILTER_DEC_SATURATE, Filter::get_filter(FILTER_DEC_SATURATE)));
    _filters.emplace(std::make_pair(FILTER_INC_LIGHTNESS, Filter::get_filter(FILTER_INC_LIGHTNESS)));
    _filters.emplace(std::make_pair(FILTER_DEC_LIGHTNESS, Filter::get_filter(FILTER_DEC_LIGHTNESS)));
//    _filters.emplace(std::make_pair(FILTER_COLORMAP, Filter::get_filter(FILTER_COLORMAP)));
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
        _probability() {
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


    init_filters();

//     init layers
    for (auto &it : _filters) {
        _layers.emplace(std::make_pair(it.first, new Layer(_img.size(), it.second)));
    }
}

void Painter::draw() {
    clock_t tStart;

    tStart = clock();
    std::map<int, double> probability = {
            {FILTER_BLUR,          0.10},
// @todo fill doesn't work
//            {FILTER_FILL,          0.08},
            {FILTER_INC_SATURATE,  0.15}, // +
            {FILTER_DEC_SATURATE,  0.15}, // +
            {FILTER_INC_LIGHTNESS, 0.15},
            {FILTER_DEC_LIGHTNESS, 0.15},
            {FILTER_BW,      0.1},
//            {FILTER_COLORMAP,      0.1},
            {FILTER_NOTHING, 1} // default
    };


    _calculate_probability(probability);
    cout << "_calculate_probability: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

//    _calculate_test_triangles();
    tStart = clock();
    // @todo optimize
    _calculate_triangles();
    cout << "_calculate_triangles: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

    tStart = clock();
    // @todo optimize
    calculate_small_triangle();
    cout << "calculate_small_triangle: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

    tStart = clock();
    std::vector<std::thread> t;

    int i = 0;
    for (auto &it: _layers) {
        t.push_back(std::move(std::thread(&Painter::drawLayer, this, std::ref(it.second), i)));
        i++;
    }

    for (std::thread &th : t) {
        if (th.joinable()) {
            th.join();
        }
    }
    cout << "drawLayer: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;
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
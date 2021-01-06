#include "Painter.h"

#include <utility>

// private
void Painter::_calculate_scale() {
    // delete empty callbacks
    for (auto it = _callbacks.begin(); it != _callbacks.end(); /* NOTHING */) {
        if (it->second == 0) {
            it = _callbacks.erase(it);
        } else {
            ++it;
        }
    }

    int n = _callbacks.size();

    for (int i = 1; i < n; i++) {
        _callbacks[i].second += _callbacks[i - 1].second;
    }

    int i = 0;
    for (auto &_callback : _callbacks) {
        cout << "callback[" << i++ << "]: " << _callback.second << endl;
    }
}

void Painter::_calculate_test_triangles() {
    int c = 0, r = 3;
    cv::Point p1[3] = {
            _point_by_coord(c, r),
            _point_by_coord(c + _step, r + _step),
            _point_by_coord(c - _step, r + _step)
    };

    _add_gradient_triangle(_get_random_layer(), p1);

    cv::Point p2[3] = {
            _point_by_coord(5, 5),
            _point_by_coord(7, 7),
            _point_by_coord(5, 7)
    };

    _add_gradient_triangle(_get_random_layer(), p2);
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

//                _add_triangle(_get_random_layer(), p1);
                _add_gradient_triangle(_get_random_layer(), p1);

                TRIANGLE p2 = {
                        _point_by_coord(c + _step, r + _step),
                        _point_by_coord(c + 2 * _step, r),
                        _point_by_coord(c, r)
                };
//                _add_triangle(_get_random_layer(), p2);
                _add_gradient_triangle(_get_random_layer(), p2);

            }
        } else {
            for (int c = 0; c < _cols * 2 * _step + 1; c += 2 * _step) {
                TRIANGLE p1 = {
                        _point_by_coord(c + _step, r),
                        _point_by_coord(c + 2 * _step, r + _step),
                        _point_by_coord(c, r + _step)
                };

//                _add_triangle(_get_random_layer(), p1);
                _add_gradient_triangle(_get_random_layer(), p1);

                TRIANGLE p2 = {
                        _point_by_coord(c, r + _step),
                        _point_by_coord(c - _step, r),
                        _point_by_coord(c + _step, r)
                };
//                _add_triangle(_get_random_layer(), p2);
                _add_gradient_triangle(_get_random_layer(), p2);
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
        if (x >= _size.width) x = _size.width - 1;

        _points.insert({cr, cv::Point(x, y)});
    }

    return _points.at(cr);
}

cv::Mat &Painter::_get_random_layer() {
    int n = _callbacks.size();

    double r_norm = (double) rand() / RAND_MAX;

    int i = 0;
    do {
        if (r_norm < _callbacks[i].second) {
            break;
        }

        i++;
    } while (i < n);

    return _layers[i];
}

void Painter::_add_triangle(cv::Mat &layer, TRIANGLE points) {
//    _triangles.push_back(points);

    fillConvexPoly(layer, points, 3, cv::Scalar(255, 255, 255), 8);
}

void Painter::_add_gradient_triangle(cv::Mat &layer, TRIANGLE points) {
//    _add_triangle(layer, points);

    cv::Mat mask = cv::Mat(layer.size(), layer.type(), cv::Scalar(0));
    _add_triangle(mask, points);

    cv::Mat gradient = cv::Mat(layer.size(), layer.type(), cv::Scalar(0));
    _gradient_section(gradient, points);

    gradient.copyTo(layer, mask);
}

void Painter::_gradient_section(cv::Mat &mask, TRIANGLE points) {
    std::sort(points, points + 3, [](cv::Point a, cv::Point b) { return a.y < b.y; });

    int ymin = points[0].y;
    int ymax = points[2].y;

    int rows = ymax - ymin + 1;

    int color_start = 0;
    int color_end = 255;
    // @todo support gradient direction

    int color;
    for (int r = ymin, i = 1; r < ymax; r++, i++) {
        color = static_cast<int> (round((double) ((color_end - color_start) * i) / rows));
        mask.row(r).setTo(cv::Scalar(color));
    }
}

void Painter::_drawLayer(int index) {
    cv::Mat roi = cv::Mat::zeros(_img.size(), _img.type());

    CALLBACK f = _callbacks[index].first;
    (this->*f)(&_img, &roi, &_layers[index]);

    cv::Mat Result = (_img & (~_layers[index])) + roi;
    Result.copyTo(_img, _layers[index]);
}

void Painter::_drawLayerAdvanced(int index) {
    cv::Mat img_f = cv::Mat::zeros(_img.size(), _img.type());

    CALLBACK f = _callbacks[index].first;
    (this->*f)(&_img, &img_f, &_layers[index]);

    cv::Mat result;
    for (int r = 0; r < _img.rows; r++) {
        cv::Rect row = cv::Rect(0, r, _img.cols, 1);
        cv::Mat roi_i(_img, row);
        cv::Mat roi_f(img_f, row);

        cv::Mat row_mask(_layers[index], row);
        cv::Mat roi_r(roi_i.size(), _img.type());

        for (int c = 0; c < roi_i.cols; c++) {
            double alpha = static_cast<double>(row_mask.at<uchar>(c)) / 255;

            for (int ch = 0; ch < roi_i.channels(); ch++) {
                roi_r.at<cv::Vec3b>(c)[ch] =
                        cv::saturate_cast<uchar>(
                                alpha * roi_i.at<cv::Vec3b>(c)[ch]
                                +
                                (1 - alpha) * roi_f.at<cv::Vec3b>(c)[ch]
                        );
            }
        }

        result.push_back(roi_r);
    }

    result.copyTo(_img, _layers[index]);
}

void Painter::_blur(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    cv::blur(*input, *output, cv::Size(10, 10));
}

void Painter::_fill(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    cv::Mat tmp;
    cv::cvtColor(*mask, tmp, cv::COLOR_RGB2GRAY);
    output->setTo(cv::mean(*input, tmp));
}

void Painter::_bw(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    cv::Mat tmp;

    cv::cvtColor(*input, tmp, cv::COLOR_RGB2GRAY);
    cv::cvtColor(tmp, tmp, cv::COLOR_GRAY2RGB);
    tmp.convertTo(tmp, CV_8UC3);

    tmp.copyTo(*output, *mask);
}

void Painter::_inc_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 1, 15);
}

void Painter::_dec_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 1, -15);
}

void Painter::_inc_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 1.1, 0);
}

void Painter::_dec_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 0.9, 0);
}

void Painter::_colorMap(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    applyColorMap(*input, *output, cv::COLORMAP_BONE);
}

void Painter::_nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    *output = *input;
}

// public
Painter::Painter(std::string filename, int step, int a) :
        _filename(std::move(filename)),
        _step(step),
        _a(a),
        _h(SQRT3 * _a / 2),
        _points(),
        _triangles(),
        _callbacks{
                CALLBACK_PAIR(&Painter::_blur, 0.2),
//                CALLBACK_PAIR(&Painter::_fill, 0.08),

                CALLBACK_PAIR(&Painter::_inc_saturate, 0.2), // +
                CALLBACK_PAIR(&Painter::_dec_saturate, 0.2), // +

                CALLBACK_PAIR(&Painter::_inc_lightness, 0.2),
                CALLBACK_PAIR(&Painter::_dec_lightness, 0.2),

                CALLBACK_PAIR(&Painter::_bw, 0.1),
//                CALLBACK_PAIR(&Painter::_colorMap, 1.0 / 100),
                CALLBACK_PAIR(&Painter::_nothing, 1) // default
//                CALLBACK_PAIR(&Painter::_bw, 1) // default
        } {


//    _img = cv::imread(_filename, 1);
    _img = cv::imread(_filename, cv::IMREAD_COLOR);

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

    _size = cv::Size(img_new_w, img_new_h);
    resize(_img, _img, _size);

    for (int i = 0; i < _callbacks.size(); i++) {
        cout << "type" << _img.type();
        _layers.push_back(cv::Mat::zeros(_img.size(), CV_8U));
    }
}

void Painter::draw() {
    _calculate_scale();
//    _calculate_test_triangles();
    _calculate_triangles();
    _calculate_small_triangle();

    std::vector<std::thread> t;
    for (int i = 0; i < _callbacks.size(); i++) {
//        _drawLayerAdvanced(i);
//        t.push_back(std::move(std::thread(&Painter::_drawLayer, this, i)));
        t.push_back(std::move(std::thread(&Painter::_drawLayerAdvanced, this, i)));
    }

    for (std::thread &th : t) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void Painter::show() {
    imshow("Image", _img);
    cv::waitKey();
}

void Painter::save(const std::string &filename) {
    imwrite(filename, _img);
}
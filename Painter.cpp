#include "Painter.h"

// private
void Painter::_calculate_scale() {
    int n = _callbacks.size();
    _scale.insert(_scale.begin(), 1, 0);
    _scale.insert(_scale.end(), 1, 1);

    for (int i = 1; i <= n; i++) {
        _scale[i] += _scale[i - 1];
    }
}

void Painter::_calculate_points() {
    int type = 0; // up|down // @todo test different modes!

    for (int r = 0; r <= _rows; r++) {
        std::vector<cv::Point> row;
        int y = round(r * _h);

        if (r % 2) {
            for (int c = 0; c <= _cols; c++) {
                int x = round(c * _a);
                if (x >= _size.width) {
                    x = _size.width - 1;
                }
                row.emplace_back(cv::Point(x, y));
            }
        } else {
            for (int c = 0; c <= _cols + 1; c++) {
                int x = c ? round((c - 0.5) * _a) : 0;
                if (x >= _size.width) {
                    x = _size.width - 1;
                }
                row.emplace_back(cv::Point(x, y));
            }
        }

        _points.push_back(row);
    }

    for (auto i : _points) {
        for (auto p : i) {
            cout << p;
        }
        cout << endl;
    }
}

int Painter::_callback() {
    int n = _callbacks.size();
    int r = -1;

    double r_norm = (double) rand() / RAND_MAX;

    for (int i = 0; i < n; i++) {
        if (_scale[i] < r_norm && r_norm <= _scale[i + 1]) {
            r = i;
        }
    }

    if (r == -1) {
        r = n;
    }

    return r;
}

void Painter::_addTriangle(cv::Point *points) {
    int index = _callback();
    cout << "callback: " << index << endl;

    _addSmallTriangle(points);

    fillConvexPoly(_layers[index], points, 3, cv::Scalar(255, 255, 255), 8);
}

void Painter::_addSmallTriangle(cv::Point *points) {
    int min = 1;
    int max = _step - 1;
    int coef = rand() % (max - min) + min;
    if (rand() % 10 > 6) {
        cv::Point smallTrianglePoints[3] = {points[0], points[1], points[2]};
//        int coef = 4;
        if (smallTrianglePoints[1].y == smallTrianglePoints[2].y) {
            smallTrianglePoints[0].x += coef * _a / (_step * 2);
            smallTrianglePoints[0].y += coef * _h / _step;
            smallTrianglePoints[2].x += coef * _a / _step;
        }

        if (smallTrianglePoints[0].y == smallTrianglePoints[1].y) {
            smallTrianglePoints[0].x += coef * _a / _step;
            smallTrianglePoints[2].x += coef * _a / (_step * 2);
            smallTrianglePoints[2].y -= coef * _h / _step;
        }

        _addTriangle(smallTrianglePoints);
//        fillConvexPoly(_layers[_callback()], smallTrianglePoints, 3, cv::Scalar(255, 255, 255), 8);
    }
}

void Painter::_drawLayer(int index) {
    cv::Mat roi = cv::Mat::zeros(_img.size(), _img.type());

    CALLBACK f = _callbacks[index];
    (this->*f)(&_img, &roi, &_layers[index]);

    cv::Mat Result = (_img & (~_layers[index])) + roi;
    Result.copyTo(_img, _layers[index]);
}

void Painter::_draw() {
    int test = 0;
    if (test) {
        int c = 1, r = 2;
        cv::Point p1[3] = {
                _points[r][c],
                _points[r + 1][c + 1],
                _points[r + 1][c],
        };
        _addTriangle(p1);
    } else {
        for (int r = 0; r < _rows; r++) {
            if (r % 2) {
                for (int c = 0; c < _cols + 1; c++) {
                    // half
                    cv::Point p1[3] = {
                            _points[r][c],
                            _points[r + 1][c + 1],
                            _points[r + 1][c],
                    };

                    _addTriangle(p1);

                    // full
                    if (c != _cols) {
                        cv::Point p2[3] = {
                                _points[r][c],
                                _points[r][c + 1],
                                _points[r + 1][c + 1],
                        };
                        _addTriangle(p2);
                    }
                }
            } else {
                for (int c = 0; c < _cols + 1; c++) {
                    // half
                    cv::Point p2[3] = {
                            _points[r][c],
                            _points[r][c + 1],
                            _points[r + 1][c],
                    };
                    _addTriangle(p2);

                    // full
                    if (c != _cols) {
                        cv::Point p1[3] = {
                                _points[r][c + 1],
                                _points[r + 1][c + 1],
                                _points[r + 1][c],
                        };

                        _addTriangle(p1);
                    }
                }
            }
        }
    }

    std::vector<std::thread> t;
    for (int i = 0; i < _callbacks.size(); i++) {
        t.push_back(std::move(std::thread(&Painter::_drawLayer, this, i)));
    }

    for (std::thread &th : t) {
        if (th.joinable()) {
            th.join();
        }
    }
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
    input->convertTo(*output, CV_8UC3, 1, 20);
}

void Painter::_dec_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 1, -20);
}

void Painter::_inc_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 1.2, 0);
}

void Painter::_dec_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    input->convertTo(*output, CV_8UC3, 0.8, 0);
}

void Painter::_nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    *output = *input;
}

// public
Painter::Painter(std::string filename, int a) :
        _filename(filename),
        _a(a),
        _h(SQRT3 * a / 2),
        _step(4),
        _callbacks{
                &Painter::_blur,
                &Painter::_fill,
                &Painter::_bw,

                &Painter::_inc_saturate,
                &Painter::_dec_saturate,

                &Painter::_inc_lightness,
                &Painter::_dec_lightness,

                &Painter::_nothing
        },
        _scale{1.0 / 6, 1.0 / 10, 1.0 / 10,
               1.0 / 6, 1.0 / 6,
               1.0 / 6, 1.0 / 6,
               1} {

    _calculate_scale();

//    for (auto i : _scale) {
//        cout << i << " ";
//    }
//    cout << endl;


//    _img = cv::imread(_filename, 1);
    _img = cv::imread(_filename, CV_LOAD_IMAGE_COLOR);

    int img_w = _img.size().width;
    int img_h = _img.size().height;

    _cols = round(img_w / _a);
    _rows = round(img_h / _h);

    int img_new_w = round(_cols * _a);
    int img_new_h = round(_rows * _h);

    cout << "w: " << img_new_w << " h: " << img_new_h << endl;
    cout << "cols: " << _cols << " rows: " << _rows << endl;
    cout << "a: " << a << " h: " << _h << endl;
    cout << "h coef: " << (double) (img_h - img_new_h) * 100 / img_h << std::setprecision(5) << "%"
         << " w coef: " << (double) (img_w - img_new_w) * 100 / img_w << std::setprecision(5) << "%" << endl;

    _size = cv::Size(img_new_w, img_new_h);
    resize(_img, _img, _size);

    _calculate_points();

    for (int i = 0; i < _callbacks.size(); i++) {
        _layers.push_back(cv::Mat::zeros(_img.size(), _img.type()));
    }

    _draw();
}

void Painter::show() {
    imshow("Image", _img);
    cv::waitKey();
}

void Painter::save(std::string filename) {
    imwrite(filename, _img);
}
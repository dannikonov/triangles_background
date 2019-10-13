#include "Painter.h"

// private
void Painter::_calculate_scale() {
    int n = (sizeof(_callbacks) / sizeof(*_callbacks));
    std::cout << "n: " << n << std::endl;

    double p[n - 1];
    p[0] = 1.0 / 3;
    p[1] = 1.0 / 4;

    _scale[0] = 0;
    _scale[n] = 1;

    for (int i = 1; i < n; i++) {
        _scale[i] += _scale[i - 1] + p[i - 1];
    }
}

void Painter::_calculate_points() {
    int type = 0; // up|down // @todo test different modes!

    for (int r = 0; r < _rows + 1; r++) {
        std::vector<cv::Point> t;//[cols + 2];
        if ((r + type) % 2) {
            for (int c = 0; c < _cols + 1; c++) {
                int x = round(c * _a);
                if (x >= _size.width) {
                    x = _size.width - 1;
                }
                int y = round(r * _h);
                t.emplace_back(cv::Point(x, y));
            }
        } else {
            for (int c = 0; c < _cols + 2; c++) {
                int x = c ? round((c - 0.5) * _a) : 0;
                if (x >= _size.width) {
                    x = _size.width - 1;
                }
                int y = round(r * _h);
                t.emplace_back(cv::Point(x, y));
            }
        }
        _points.push_back(t);

    }
}

int Painter::_callback() {
    int n = (sizeof(_callbacks) / sizeof(*_callbacks));
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

void Painter::_drawTriangle(cv::Point *points, cv::Mat *m) {
    std::cout << "p: " << points[0] << points[1] << points[2] << std::endl;
    cv::Mat mask = cv::Mat::zeros(m->size(), m->type());

    const cv::Point *ppt[1] = {points};
    int npt[] = {3};

    fillPoly(mask, ppt, npt, 1, cv::Scalar(255, 255, 255), 8);

    cv::Mat roi = cv::Mat::zeros(m->size(), m->type());;

    int n = _callback();
    std::cout << "callback: " << n << std::endl;
    if (n == (sizeof(_callbacks) / sizeof(*_callbacks))) {
        return;
    } else {
        CALLBACK f = _callbacks[n];
        (this->*f)(&_img, &roi, &mask);
    }
    cv::Mat Result = (*m & (~mask)) + roi;
    Result.copyTo(*m, mask);
//    }
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
        _drawTriangle(p1, &_img);
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

                    _drawTriangle(p1, &_img);

                    // full
                    if (c != _cols) {
                        cv::Point p2[3] = {
                                _points[r][c],
                                _points[r][c + 1],
                                _points[r + 1][c + 1],
                        };
                        _drawTriangle(p2, &_img);
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
                    _drawTriangle(p2, &_img);

                    // full
                    if (c != _cols) {
                        cv::Point p1[3] = {
                                _points[r][c + 1],
                                _points[r + 1][c + 1],
                                _points[r + 1][c],
                        };

                        _drawTriangle(p1, &_img);
                    }
                }
            }
        }
    }
}

void Painter::_blur(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    cv::blur(*input, *output, cv::Size(10, 10));
}

void Painter::_fill(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    cv::Mat newmask;
    cv::cvtColor(*mask, newmask, cv::COLOR_RGB2GRAY);
    output->setTo(cv::mean(*input, newmask));
}

void Painter::_nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
    *output = *input;
}

// public
Painter::Painter(std::string filename, int a) :
        _filename(filename),
        _a(a),
        _h(SQRT3 * a / 2),
        _callbacks{
                &Painter::_blur,
                &Painter::_fill,
                &Painter::_nothing
        } {

    _calculate_scale();

    _img = cv::imread(_filename, CV_LOAD_IMAGE_COLOR);

    int img_w = _img.size().width;
    int img_h = _img.size().height;

    _cols = round(img_w / _a);
    _rows = round(img_h / _h);

    int img_new_w = round(_cols * _a);
    int img_new_h = round(_rows * _h);

    std::cout << "w: " << img_new_w << " h: " << img_new_h << std::endl;
    std::cout << "cols: " << _cols << " rows: " << _rows << std::endl;
    std::cout << "a: " << a << " h: " << _h << std::endl;
    std::cout << "h coef: " << (double) (img_h - img_new_h) * 100 / img_h << std::setprecision(5) << "%"
              << " w coef: " << (double) (img_w - img_new_w) * 100 / img_w << std::setprecision(5) << "%" << std::endl;

    _size = cv::Size(img_new_w, img_new_h);
    resize(_img, _img, _size);

    _calculate_points();

    _draw();
}

void Painter::show() {
    imshow("Image", _img);
    cv::waitKey();
}

void Painter::save(std::string filename) {
    imwrite(filename, _img);
}
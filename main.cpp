#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/thread/thread.hpp>

#define SQRT3 1.73205080756887729352

void drawCircle(int x, int y, int r, int border, cv::Mat *m) {
    cv::Mat mask = cv::Mat::zeros(m->size(), m->type());
    circle(mask, cv::Point(x, y), r, cv::Scalar(255, 255, 255), -1, 8, 0);
    cv::Mat roi;
    cv::blur(*m & mask, roi, cv::Size(11, 11), cv::Point(-1, -1), border);
    cv::Mat Result = (*m & (~mask)) + roi;
    Result.copyTo(*m, mask);
}

void drawTriangle(double x, double y, double a, double angle, cv::Mat *m) {
    cv::Mat mask = cv::Mat::zeros(m->size(), m->type());

    cv::Point rook_points[3];
    rook_points[0] = cv::Point(x, y);
    rook_points[1] = cv::Point(x + a, y);
    rook_points[2] = cv::Point(x + a / 2, y + SQRT3 * a / 2);

    const cv::Point *ppt[1] = {rook_points};
    int npt[] = {3};

    fillPoly(mask, ppt, npt, 1, cv::Scalar(255, 255, 255), 8);

    if (angle) {
        cv::Point2f pc(x + a / 2., y + SQRT3 * a / 4);
        cv::Mat dst = cv::getRotationMatrix2D(pc, angle, 1.0);
        cv::Mat dst2;
        cv::warpAffine(mask, mask, dst, mask.size());
    }

    cv::Mat roi;
    cv::blur(*m & mask, roi, cv::Size(11, 11), cv::Point(-1, -1));
    cv::Mat Result = (*m & (~mask)) + roi;
    Result.copyTo(*m, mask);
}


int main(int argc, char *argv[]) {
    int CORES = static_cast<int>(boost::thread::hardware_concurrency());
    std::cout << CORES;

//    return 0;
    std::string filename = "";
    int a = 0;
    double h;
    if (argc >= 3) {
        filename = argv[1];
        a = atoi(argv[2]);
        h = SQRT3 * a / 2;
    }

    if (filename.empty() || a == 0) {
        return 0;
    }

    cv::Mat m = cv::imread(filename, CV_LOAD_IMAGE_COLOR);

    int img_h = m.size().height;
    int img_w = m.size().width;

    int cols = ceil(img_w / a);
    int rows = ceil(img_h / h);

    cv::Size size(cols * a, rows * h);
    resize(m, m, size);

    for (int r = 0; r < rows; r += 2) {
        for (int c = 0; c < cols + 1; c++) {
            drawTriangle(c * a, r * h, a, 0, &m);
            drawTriangle(-a / 2 + c * a, r * h, a, 180, &m);
            drawTriangle(-a / 2 + c * a, (r + 1) * h, a, 0, &m);
            drawTriangle(c * a, (r + 1) * h, a, 180, &m);
        }
    }

//    drawCircle(50, 550, 50, cv::BORDER_REPLICATE, &m);

    imshow("Blur with mask", m);

    cv::waitKey();

    return 0;
}
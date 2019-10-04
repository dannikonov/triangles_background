#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <time.h>
#include <thread>
#include <iomanip>

#define SQRT3 1.73205080756887729352

void drawTriangle(cv::Point *points, cv::Mat *m) {
    cv::Mat mask = cv::Mat::zeros(m->size(), m->type());

    const cv::Point *ppt[1] = {points};
    int npt[] = {3};

    fillPoly(mask, ppt, npt, 1, cv::Scalar(255, 255, 255), 8);

    cv::Mat roi;
//    cv::blur(*m & mask, roi, cv::Size(4, 4));
    cv::blur(*m, roi, cv::Size(14, 14));

    cv::Mat Result = (*m & (~mask)) + roi;
    Result.copyTo(*m, mask);
}

int main(int argc, char *argv[]) {
    time_t start, end;
    time(&start);

    int CORES = static_cast<int>(boost::thread::hardware_concurrency());
    clock_t tStart = clock();

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

    int img_w = m.size().width;
    int img_h = m.size().height;

    int cols = round(img_w / a);
    int rows = round(img_h / h);

    int img_new_w = round(cols * a);
    int img_new_h = round(rows * h);

    std::cout << "w: " << img_new_w << " h: " << img_new_h << std::endl;
    std::cout << "cols: " << cols << " rows: " << rows << std::endl;
    std::cout << "a: " << a << " h: " << h << std::endl;
    std::cout << "h coef: " << (double) (img_h - img_new_h) * 100 / img_h << std::setprecision(5) << "%"
              << " w coef: " << (double) (img_w - img_new_w) * 100 / img_w << std::setprecision(5) << "%" << std::endl;

    int type = 0; // up|down
    std::vector<std::vector<cv::Point>> points; // points[rows + 1][cols + 2]
    for (int r = 0; r < rows + 1; r++) {
        std::vector<cv::Point> t;//[cols + 2];
        if ((r + type) % 2) {
            for (int c = 0; c < cols + 1; c++) {
                int x = round(c * a);
                if (x >= img_new_w) {
                    x = img_new_w - 1;
                }
                int y = round(r * h);
                t.emplace_back(cv::Point(x, y));
            }
        } else {
            for (int c = 0; c < cols + 2; c++) {
                int x = c ? round((c - 0.5) * a) : 0;
                if (x >= img_new_w) {
                    x = img_new_w - 1;
                }
                int y = round(r * h);
                t.emplace_back(cv::Point(x, y));
            }
        }
        points.push_back(t);
    }


    cv::Size size(img_new_w, img_new_h);
    resize(m, m, size);

    std::vector<std::thread> t;

    for (int r = 0; r < rows; r++) {
        if (r % 2) {
            for (int c = 0; c < cols + 1; c++) {
                // half
                cv::Point p1[3] = {
                        points[r][c],
                        points[r + 1][c + 1],
                        points[r + 1][c],
                };
                drawTriangle(p1, &m);
                std::cout << "{" << r << "," << c << "}" << p1[0] << p1[1] << p1[2] << std::endl;

                // full
                if (c != cols) {
                    cv::Point p2[3] = {
                            points[r][c],
                            points[r][c + 1],
                            points[r + 1][c + 1],
                    };
                    drawTriangle(p2, &m);
                    std::cout << "{" << r << "," << c << "}" << p2[0] << p2[1] << p2[2] << std::endl;
                }
            }
        } else { // 0, 2
            for (int c = 0; c < cols + 1; c++) {
                // half
                cv::Point p2[3] = {
                        points[r][c],
                        points[r][c + 1],
                        points[r + 1][c],
                };
                std::cout << "{" << r << "," << c << "}" << p2[0] << p2[1] << p2[2] << std::endl;
                drawTriangle(p2, &m);

                // full
                if (c != cols) {
                    cv::Point p1[3] = {
                            points[r][c + 1],
                            points[r + 1][c + 1],
                            points[r + 1][c],
                    };
                    drawTriangle(p1, &m);
                    std::cout << "{" << r << "," << c << "}" << p1[0] << p1[1] << p1[2] << std::endl;
                }
            }
        }
    }

//    for (int r = 0; r < rows; r++) {
//        for (int c = 0; c < cols; c++) {
//            t.push_back(std::move(std::thread(draw, r, c, h, a, &m)));
//        }
//        for (std::thread &th : t) {
//            if (th.joinable()) {
//                th.join();
//            }
//        }
//    }

    for (int r = 0; r < points.size(); r++) {
        for (int c = 0; c < points[r].size(); c++) {
            std::cout << "{" << r << "," << c << "}" << points[r][c] << "    ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "Time taken: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << std::endl;
    time(&end);
    std::cout << std::fixed << double(end - start) << std::setprecision(5) << std::endl;

    imshow("Blur with mask", m);

    cv::waitKey();

    return 0;
}
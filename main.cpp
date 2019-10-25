#include "Painter.h"

#include <boost/thread/thread.hpp>
#include <time.h>
//#include <thread>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    time_t start, end;
    time(&start);

    int CORES = static_cast<int>(boost::thread::hardware_concurrency());
    clock_t tStart = clock();

    std::string filename = "";
    int a = 0;
    if (argc >= 3) {
        filename = argv[1];
        a = atoi(argv[2]);
    }

    if (filename.empty() || a == 0) {
        return 0;
    }

    Painter *p = new Painter(filename, a);
    p->show();
//    p->save("/tmp/test2_out.jpg");

    delete p;

//    std::vector<std::thread> t;
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

//    cout << endl << "points:" << endl;
//    for (int r = 0; r < points.size(); r++) {
//        for (int c = 0; c < points[r].size(); c++) {
//            cout << "{" << r << "," << c << "}" << points[r][c] << "    ";
//        }
//        cout << endl;
//    }

    cout << endl << "Time taken: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;
    time(&end);
    cout << std::fixed << double(end - start) << std::setprecision(5) << endl;


    return 0;
}
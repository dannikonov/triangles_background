#include "Painter.h"

#include <time.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    time_t start, end;
    time(&start);

//    int CORES = static_cast<int>(boost::thread::hardware_concurrency());
    clock_t tStart = clock();

    std::string filename = "";
    std::string outfilename = "";

    int a = 0;
    if (argc >= 4) {
        filename = argv[1];
        outfilename = argv[2];
        a = atoi(argv[3]);
    }

    if (filename.empty() || a == 0) {
        return 0;
    }

    Painter *p = new Painter(filename, 4, a);
//    p->show();
    p->save(outfilename);

    delete p;

    cout << endl << "Time taken: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;
    time(&end);
    cout << std::fixed << double(end - start) << std::setprecision(5) << endl;

    return 0;
}
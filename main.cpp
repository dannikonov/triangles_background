#include "Painter.h"
#include "Example.h"


#include <ctime>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// start arguments
// for test mode
// : example
// for real mode
// : "/tmp/test.jpg" "/tmp/test_t.jpg" 1400

int main(int argc, char *argv[]) {
    std::string target = "/tmp/tri";
    if (!boost::filesystem::exists(target)) {
        boost::filesystem::create_directory(target);
    }

//    int CORES = static_cast<int>(boost::thread::hardware_concurrency());
    clock_t tStart = clock();

    if (argc == 2) {
        // example mode
        auto *p = new Example("/tmp/tri/test.jpg");

        p->bw_with_mask();
//        p->mask_use_alpha();
        p->show();

        return 0;
    }

    if (argc >= 4) {

        int a = 0;
        std::string filename = argv[1];
        std::string outfilename = argv[2];

        a = atoi(argv[3]);

        if (filename.empty() || a == 0) {
            return 0;
        }

        auto *p = new Painter(filename, 4, a);
        p->draw();
//        p->show();
        p->save(outfilename);

        delete p;
    }

    cout << endl << "Time taken: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << endl;

    return 0;
}
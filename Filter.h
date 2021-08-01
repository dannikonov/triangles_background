#ifndef TRIANGULATE_FILTER_H
#define TRIANGULATE_FILTER_H

#define FILTER_BLUR 1
#define FILTER_FILL 2

#define FILTER_INC_SATURATE 3
#define FILTER_DEC_SATURATE 4

#define FILTER_INC_LIGHTNESS 5
#define FILTER_DEC_LIGHTNESS 6

#define FILTER_BW 7
#define FILTER_COLORMAP 8

#define FILTER_NOTHING 9999

using FILTER = void (*)(cv::Mat *input, cv::Mat *output, cv::Mat *mask);

class Filter {
//    std::function<void(cv::Mat *input, cv::Mat *output, cv::Mat *mask)> FILTER;

public:
    static FILTER get_filter(int filter) {
        switch (filter) {
            case FILTER_BLUR:
                return Filter::blur;
            case FILTER_FILL:
                return Filter::fill;
            case FILTER_INC_SATURATE:
                return Filter::inc_saturate;
            case FILTER_DEC_SATURATE:
                return Filter::dec_saturate;
            case FILTER_INC_LIGHTNESS:
                return Filter::inc_lightness;
            case FILTER_DEC_LIGHTNESS:
                return Filter::dec_lightness;
            case FILTER_BW:
                return Filter::bw;
            case FILTER_COLORMAP:
                return Filter::colormap;
            default:
                return Filter::nothing;
        }
    }

private:
    static void nothing(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call nothing" << std::endl;
        *output = *input;
    };

    static void blur(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call blur" << std::endl;
        cv::blur(*input, *output, cv::Size(10, 10));
    };

    static void fill(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        cv::Mat tmp;
        cv::cvtColor(*mask, tmp, cv::COLOR_RGB2GRAY);
        output->setTo(cv::mean(*input, tmp));
    };

    static void bw(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call bw" << std::endl;
        cv::Mat tmp;

        cv::cvtColor(*input, tmp, cv::COLOR_RGB2GRAY);
        cv::cvtColor(tmp, tmp, cv::COLOR_GRAY2RGB);
        tmp.convertTo(tmp, CV_8UC3);

        tmp.copyTo(*output, *mask);
    };

    static void inc_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call inc_saturate" << std::endl;
        input->convertTo(*output, CV_8UC3, 1, 15);
    };

    static void dec_saturate(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call dec_saturate" << std::endl;
        input->convertTo(*output, CV_8UC3, 1, -15);
    };

    static void inc_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call inc_lightness" << std::endl;
        input->convertTo(*output, CV_8UC3, 1.1, 0);
    };

    static void dec_lightness(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call dec_lightness" << std::endl;
        input->convertTo(*output, CV_8UC3, 0.9, 0);
    };

    static void colormap(cv::Mat *input, cv::Mat *output, cv::Mat *mask) {
        std::cout << "call colormap" << std::endl;
        applyColorMap(*input, *output, cv::COLORMAP_BONE);
    };


};

#endif //TRIANGULATE_FILTER_H

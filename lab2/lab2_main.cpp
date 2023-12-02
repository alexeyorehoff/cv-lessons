#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utils/logger.hpp"
#include <iostream>
#include <chrono>

double compare_img(cv::Mat first, cv::Mat second) {
    assert(first.cols == second.cols && first.rows == second.rows);
    double res = 0;
    for (int x = 0; x < first.cols - 1; x++) {
        for (int y = 0; y < first.rows - 1; y++) {
            auto first_value = first.at<uint8_t>(x, y);
            auto second_value = second.at<uint8_t>(x, y);
            res += fabs(first_value - second_value) / second_value;
        }
    }
    return res / (first.cols * first.rows);
}

cv::Mat box_filter(cv::Mat img, ushort box_size=3) {
    cv::Mat res = img.clone();
    assert(img.channels() == 1 && box_size % 2 == 1);
    for (int y = 0; y <= img.rows - box_size; y++) {
        for (int x = 0; x <= img.cols - box_size; x++) {
            cv::Mat roi_rect = img(cv::Rect(x, y, box_size, box_size));
            res.at<uint8_t>(y + box_size / 2, x + box_size / 2) =
                    (uint8_t)(cv::sum(roi_rect) / (box_size * box_size))[0];
        }
    }
    return res;
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);

    cv::Mat original = cv::imread("../lab2/lenna.png");
    cv::Mat grayscale;
    cv::cvtColor(original, grayscale, cv::COLOR_BGR2GRAY);
    cv::imshow("original", grayscale);
    cv::setBreakOnError(false);

    using std::chrono::steady_clock;

    auto start = steady_clock::now();
    auto diy_box_filter = box_filter(grayscale, 5);
    std::cout << "DIY box filter execution time: " << steady_clock::now() - start << std::endl;

    cv::Mat box_filter_opencv;
    start = steady_clock::now();
    cv::blur(grayscale, box_filter_opencv, {5, 5});
    std::cout << "opencv box filter execution time: " << steady_clock::now() - start << std::endl;

    std::cout << "Difference between DIY and opencv methods: " << compare_img(diy_box_filter, box_filter_opencv) << std::endl;
    cv::imshow("box filter", box_filter(grayscale, 5));
    cv::waitKey(0);
    return 0;
}
#include <iostream>
#include <chrono>
#include <algorithm>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utils/logger.hpp"


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
    return 100 - 100 * res / (first.cols * first.rows);
}

cv::Mat img_diff(cv::Mat first, cv::Mat second) {
    cv::Mat diff;
    cv::absdiff(first, second, diff);
    return diff;
}

cv::Mat log_amplify(cv::Mat src) {
    cv::Mat ampl = src.clone();
    ampl.convertTo(ampl, CV_32F);
    cv::log(ampl + 1, ampl);
    cv::normalize(ampl, ampl, 0, 255, cv::NORM_MINMAX);
    ampl.convertTo(ampl, CV_32F);
    return ampl;
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

cv::Mat unsharp_mask(cv::Mat img, double alpha = 0.5) {
    return img + alpha * (img - box_filter(img));
}

cv::Mat unsharp_mask_gaussian(cv::Mat img, double alpha = 0.5) {
    cv::Mat gaussian;
    cv::GaussianBlur(img, gaussian, {5, 5}, (0, 0));
    return img + alpha * (img - gaussian);
}


cv::Mat laplace_filter(cv::Mat img, int ksize = 3) {
    assert(!img.empty() && img.channels() == 1 && ksize % 2 != 0 && ksize >= 3);
    cv::Mat laplace;
    cv::Mat kernel = cv::Mat::ones(ksize, ksize, CV_32F) * -1;
    kernel.at<float>(ksize / 2, ksize / 2) = (float)(ksize * ksize - 1);
    filter2D(img, laplace, -1, kernel);
    return laplace;
}


cv::Mat unsharp_laplasian(cv::Mat img, double alpha = 0.3) {
    return img + alpha * laplace_filter(img);
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);
    using std::chrono::steady_clock;

    cv::Mat original = cv::imread("../lab2/lenna.png");
    cv::Mat grayscale;
    cv::cvtColor(original, grayscale, cv::COLOR_BGR2GRAY);

    auto start = steady_clock::now();
    auto diy_box_filter = box_filter(grayscale, 5);
    std::cout << "DIY box filter execution time: " << steady_clock::now() - start << std::endl;

    cv::Mat box_filter_opencv;
    start = steady_clock::now();
    cv::blur(grayscale, box_filter_opencv, {5, 5});
    std::cout << "opencv box filter execution time: " << steady_clock::now() - start << std::endl;

    std::cout << "Difference between DIY and opencv methods: " << compare_img(diy_box_filter, box_filter_opencv) << std::endl;

    cv::Mat gaussian;
    cv::GaussianBlur(grayscale, gaussian, {5, 5}, (0, 0));

    std::cout << "Difference between DIY and gaussian methods: " << compare_img(diy_box_filter, gaussian) << std::endl;

    cv::imshow("grayscale original", grayscale);
    cv::imshow("diy box filter", box_filter(grayscale, 5));
    cv::imshow("opencv box filter", box_filter_opencv);

    cv::imshow("gaussian blur", gaussian);
    cv::imshow("gaussian box diff", img_diff(diy_box_filter, gaussian));
    cv::imshow("log difference", log_amplify(img_diff(diy_box_filter, gaussian)));

    cv::Mat unsharp_box = unsharp_mask(grayscale);
    cv::Mat unsharp_gaussian = unsharp_mask_gaussian(grayscale);

    cv::imshow("us box", unsharp_box);
    cv::imshow("us gaussian", unsharp_gaussian);

    cv::imshow("box gaussian diff", log_amplify(img_diff(unsharp_box, unsharp_gaussian)));

    cv::Mat laplace = laplace_filter(grayscale);
    cv::imshow("Laplace", laplace);
    cv::imshow("us_laplace", unsharp_laplasian(grayscale));
    cv::waitKey(0);
    return 0;
}

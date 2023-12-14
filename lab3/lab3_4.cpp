#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <vector>

std::vector<std::vector<cv::Point>> find_contours(cv::Mat image) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

cv::Mat find_defects(cv::Mat image, cv::Mat standard_img) {
    cv::Mat res = image.clone(), in_range;
    cv::inRange(image, cv::Scalar(0, 0, 0), cv::Scalar(250, 250, 250), in_range);

    std::vector<std::vector<cv::Point>> contours, standard_contour;
    cv::findContours(standard_img, standard_contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(in_range, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    auto standard = *std::max_element(standard_contour.begin(), standard_contour.end(),
                                      [](const auto& a, const auto& b) { return cv::contourArea(a) < cv::contourArea(b); });

    for (auto& contour: contours) {
        if (cv::contourArea(contour) > 1000) {
            double sim = cv::matchShapes(contour, standard, cv::CONTOURS_MATCH_I2, 0);
            cv::putText(res, sim < 0.5 ? "good" : "bad", cv::Point(contour[0]),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0,
                        sim < 0.5 ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);
        }
    }
    return res;
}

int main() {
    cv::imshow("gk_detect", find_defects(cv::imread("../lab3/img/task4/gk.jpg"),
                                         cv::imread("../lab3/img/task4/gk_tmplt.jpg", cv::IMREAD_GRAYSCALE)));
    cv::waitKey();
    cv::destroyAllWindows();
    return 0;
}

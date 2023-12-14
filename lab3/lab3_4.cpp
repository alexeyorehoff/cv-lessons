#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <vector>
#include <iostream>


std::vector<std::vector<cv::Point>> find_contours(cv::Mat image) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>> res;
    cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (auto& contour: contours) if (cv::contourArea(contour) > 600) res.push_back(contour);
    return res;
}

double similarity(std::vector<cv::Point> first, std::vector<cv::Point> second) {
    double area1 = cv::contourArea(first);
    double area2 = cv::contourArea(second);
    double perimeter1 = cv::arcLength(first, true);
    double perimeter2 = cv::arcLength(second, true);
    return abs(area2 - area1) / area2 + abs(perimeter2 - perimeter1) / perimeter2;
}

cv::Point get_contour_center(std::vector<cv::Point> contour) {
    cv::Moments moments = cv::moments(contour);
    int cX = int(moments.m10 / moments.m00);
    int cY = int(moments.m01 / moments.m00);
    return {cX, cY};
}

std::vector<cv::Point> get_largest_contour(std::vector<std::vector<cv::Point>> contours) {
    double max_area = 0;
    int max_area_index = -1;
    for (int idx = 0; idx < contours.size(); ++idx) {
        double area = cv::contourArea(contours[idx]);
        if (area > max_area) {
            max_area = area;
            max_area_index = (int)idx;
        }
    }
    return contours[max_area_index];
}

cv::Mat find_defects(cv::Mat image, cv::Mat standard_img) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>> standard_contour;

    cv::Mat res = image.clone(), in_range;
    cv::inRange(image, cv::Scalar(0, 0, 0), cv::Scalar(250, 250, 250), in_range);
    cv::extractChannel(standard_img, standard_img, 0);

    cv::findContours(standard_img, standard_contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(in_range, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    auto standard = get_largest_contour(standard_contour);

    for (auto& contour: contours) {
        double sim = similarity(contour, standard);
        cv::Point center = get_contour_center(contour);
        std::cout << sim << std::endl;
        cv::addText(res, std::to_string(sim), center, "Calibri");
    }

    return res;
}


int main() {

    cv::imshow("gk_detect", find_defects(cv::imread("../lab3/img/task4/gk.jpg"),
                                         cv::imread("../lab3/img/task4/gk_tmplt.jpg")));
    cv::waitKey();
    cv::destroyAllWindows();
    return 0;
}

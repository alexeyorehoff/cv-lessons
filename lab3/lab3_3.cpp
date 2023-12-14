#include "opencv2/core.hpp"
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

cv::Vec2i get_contour_center(std::vector<cv::Point> contour) {
    cv::Moments moments = cv::moments(contour);
    int cX = int(moments.m10 / moments.m00);
    int cY = int(moments.m01 / moments.m00);
    return {cX, cY};
}

int vec_length(cv::Vec2i first, cv::Vec2i second) {
    return (int)sqrt(pow(first[0] - second[0], 2) + pow(first[0] - second[0], 2));
}

std::vector<cv::Point> nearest_contour(std::vector<std::vector<cv::Point>> contours, cv::Vec2i center) {
    int min_length = INT_MAX;
    int restricted_zone = 50;
    std::vector<cv::Point> nearest;
    for (auto& contour: contours) {
        cv::Vec2i cont_center = get_contour_center(contour);
        int length = vec_length(cont_center, center);
        if (length < min_length and length > restricted_zone) {
            min_length = length;
            nearest = contour;
        }
    }
    return nearest;
}

cv::Mat detect(cv::Mat img) {
    cv::Mat blur, hsv, light, res;
    cv::blur(img, blur, {10, 10});
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar red_range_lower(0, 20, 20);
    cv::Scalar red_range_upper(15, 255, 255);

    cv::Scalar green_range_lower(60, 50, 50);
    cv::Scalar green_range_upper(75, 255, 255);

    cv::Scalar blue_range_lower(85, 30, 30);
    cv::Scalar blue_range_upper(150, 255, 255);

    cv::Mat red, green, blue;

    cv::inRange(hsv, red_range_lower, red_range_upper, red);
    cv::inRange(hsv, green_range_lower, green_range_upper, green);
    cv::inRange(hsv, blue_range_lower, blue_range_upper, blue);

    res = img.clone();
    auto red_contours = find_contours(red);
    auto green_contours = find_contours(green);
    auto blue_contours = find_contours(blue);

    cv::drawContours(res, find_contours(red), -1, cv::Scalar(0, 0, 255), 3);
    cv::drawContours(res, find_contours(green), -1, cv::Scalar(0, 255, 0), 3);
    cv::drawContours(res, find_contours(blue), -1, cv::Scalar(255, 0, 0), 3);

    cv::inRange(blur, cv::Scalar(240, 240, 240), cv::Scalar(255, 255, 255), light);
    auto light_center = get_contour_center(find_contours(light)[0]);

    cv::circle(res, get_contour_center(nearest_contour(red_contours, light_center)), 5, cv::Scalar(0, 0, 255), -1);
    cv::circle(res, get_contour_center(nearest_contour(green_contours, light_center)), 5, cv::Scalar(0, 255, 0), -1);
    cv::circle(res, get_contour_center(nearest_contour(blue_contours, light_center)), 5, cv::Scalar(255, 0, 0), -1);

    return red;
}


int main() {
    cv::VideoCapture cap("../lab3/img/task3/vid_1.mp4");
    if(!cap.isOpened()){
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }
    cv::Mat frame;
    cv::namedWindow("detector");

    while (true) {
        if (!cap.read(frame)) {
            cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
        }
        cv::imshow("detector", detect(frame));
        if (cv::waitKey(50) == 27) break;
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}

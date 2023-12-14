#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <vector>
#include <iostream>


static int threshold_level = 50;
cv::Mat original;
cv::VideoCapture cap;

cv::Mat detect(cv::Mat img) {
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
    cv::Mat hue;
    cv::extractChannel(hsv, hue, 0);
    cv::Mat thresh;
    cv::threshold(hue, thresh, threshold_level, 255, cv::THRESH_BINARY_INV);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    double max_area = 0;
    int max_area_index = -1;
    for (size_t idx = 0; idx < contours.size(); ++idx) {
        double area = cv::contourArea(contours[idx]);
        if (area > max_area) {
            max_area = area;
            max_area_index = (int)idx;
        }
    }
    cv::Mat contourImg = img.clone();
    if (max_area_index >= 0) {
        cv::drawContours(contourImg, contours, max_area_index, cv::Scalar(0, 0, 0), 2);

        // Нахождение и отображение центра контура
        cv::Moments moments = cv::moments(contours[max_area_index]);
        int cX = int(moments.m10 / moments.m00);
        int cY = int(moments.m01 / moments.m00);
        cv::circle(contourImg, cv::Point(cX, cY), 5, cv::Scalar(0, 0, 0), -1);
    }
    return contourImg;
}

static void photo_callback(int, void*) {
    cv::imshow("photo_detector", detect(original));
}


int main() {
    original = cv::imread("../lab3/img/task2/img2.jpg");

    cv::namedWindow("photo_detector");
    cv::createTrackbar("threshold", "photo_detector", &threshold_level, 255,
                       photo_callback);
    cv::waitKey();
    cv::destroyAllWindows();
    return 0;
}

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <vector>
#include <iostream>


static int threshold_level = 150;
cv::Mat original;
cv::VideoCapture cap;

cv::Mat detect(cv::Mat img) {
    cv::Mat grayscale;
    cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    cv::Mat thresh;
    cv::threshold(grayscale, thresh, threshold_level, 255, cv::THRESH_BINARY);

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
        cv::drawContours(contourImg, contours, max_area_index, cv::Scalar(0, 255, 0), 2);

        // Нахождение и отображение центра контура
        cv::Moments moments = cv::moments(contours[max_area_index]);
        int cX = int(moments.m10 / moments.m00);
        int cY = int(moments.m01 / moments.m00);
        cv::circle(contourImg, cv::Point(cX, cY), 5, cv::Scalar(0, 0, 255), -1);
    }
    return contourImg;
}

static void photo_callback(int, void*) {
    cv::imshow("photo_detector", detect(original));
}


int main() {
    original = cv::imread("../lab3/img/allababah/ig_0.jpg");
    cap = cv::VideoCapture("../lab3/img/allababah/v_1.mp4");

    if(!cap.isOpened()){
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }
    cv::Mat frame;
    cap >> frame;

    cv::namedWindow("photo_detector");
    cv::namedWindow("video_detector");

    cv::createTrackbar("threshold", "photo_detector", &threshold_level, 255,
                       photo_callback);
    cv::createTrackbar("threshold", "video_detector", &threshold_level, 255);
    while (true) {

        cap >> frame;

        if (frame.empty()) {
            cap.set(cv::CAP_PROP_POS_FRAMES, 1);
            cap >> frame;
        }
        cv::imshow("video_detector", detect(frame));

        if (cv::waitKey(5) == 27) break;
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}

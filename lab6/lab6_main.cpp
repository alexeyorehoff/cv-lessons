#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <vector>


const char* source = "../lab6/videos/2.avi";
const char* slider_win_name = "thresh_setup";


static int low_hue = 120;
static int high_hue = 255;
static int low_sat = 150;
static int high_sat = 255;
static int low_val = 0;
static int high_val = 255;


cv::Mat preprocess_frame(const cv::Mat &frame) {
    cv::Mat res = frame.clone();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::inRange(frame, cv::Scalar(low_hue, low_sat, low_val),
                cv::Scalar(high_hue, high_sat, high_val), res);
    for (int col = 0; col < res.cols; col++) {
        auto column = res.col(col);
    }
    return res;
}


int main() {
    cv::VideoCapture cap(source);
    cv::Mat frame;
    cv::namedWindow("lab6");


    cv::namedWindow(slider_win_name); // Создание окна для управляющих элементов

    cv::createTrackbar("Low Hue", slider_win_name, &low_hue, 255);
    cv::createTrackbar("High Hue", slider_win_name, &high_hue, 255);
    cv::createTrackbar("Low Saturation", slider_win_name, &low_sat, 255);
    cv::createTrackbar("High Saturation", slider_win_name, &high_sat, 255);
    cv::createTrackbar("Low Value", slider_win_name, &low_val, 255);
    cv::createTrackbar("High Value", slider_win_name, &high_val, 255);


    while (true) {
        if (!cap.read(frame)) {
            cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        cv::Mat result = preprocess_frame(frame);

        cv::imshow("original", frame);
        cv::imshow("lab6",result);
        if (cv::waitKey(50) == 27) break;
    }
    cv::destroyAllWindows();
    return 0;
}
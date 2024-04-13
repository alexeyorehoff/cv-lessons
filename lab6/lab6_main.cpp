#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

const char* source = "../lab6/videos/2.avi";
const char* slider_win_name = "thresh_setup";


static int low_hue = 230;
static int high_hue = 255;
static int low_sat = 150;
static int high_sat = 255;
static int low_val = 0;
static int high_val = 255;

const float fovx = 74;
const float focal = (float)tan(fovx / 2 / 180 * CV_PI);  // focal distance
const float cam_shift = 127;           // Distance from camera axis to laser plane

const int grid_width = 1500;           // Window size in mm before scaling
const int grid_heigth = 2200;
const float grid_scale_factor = 0.5;   // Scaling. When set to 1, 1px = 1mm
const int grid_step = 50;              // Step of grid lines in mm


cv::Mat preprocess_frame(const cv::Mat &frame) {
    // Processes frame from camera to get 1px thick line
    cv::Mat res = frame.clone();
    cv::inRange(frame, cv::Scalar(low_hue, low_sat, low_val),
                cv::Scalar(high_hue, high_sat, high_val), res);

    for (int col = 0; col < res.cols; col++) {          // Filter each column to have one averaged pixel
        auto column = res.col(col);
        int sum_row = 0;
        int count = 0;
        for (int row = 0; row < res.rows; row++) if (column.at<uchar>(row)) {
                sum_row += row;
                count++;
        }
        if (count) {
            column.setTo(0);                      // Set all pixels to black
            column.at<uchar>(sum_row / count) = 255; // Set one pixel to white
        }
    }
    return res;
}


std::vector<cv::Point3f> frame2points(cv::Mat frame) {
    cv::Mat flipped, nonzero;
    cv::flip(frame, flipped, 0);
    cv::findNonZero(flipped({0, 0, frame.cols, frame.rows / 2}), nonzero);
    std::vector<cv::Point3f> res;
    float center_x = (float)frame.cols / 2.0f;
    float center_y = (float)frame.rows / 2.0f;

    for (int idx = 0; idx < nonzero.total(); idx++) {
        cv::Point point = nonzero.at<cv::Point>(idx);

        float dx = ((float)point.x - center_x) / (float)frame.rows;
        float dy = (center_y - (float)point.y) / (float)frame.cols;

        float depth = cam_shift / dy * focal;
        float x = dx / dy * cam_shift;
        res.emplace_back(x, 0, depth);
    }
    return res;
}


cv::Mat draw_points(std::vector<cv::Point3f> points) {
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    for (const auto& point : points) {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minZ = std::min(minZ, point.z);
        maxZ = std::max(maxZ, point.z);
    }

    std::cout << "\nx values - " << minX << " - " << maxX << "\n" << "z values - " << minZ << " - " << maxZ << "\n";

    int s_width = int(grid_width * grid_scale_factor);  // Scaled values
    int s_height = int(grid_heigth * grid_scale_factor);
    int s_step = int(grid_step * grid_scale_factor);

    cv::Mat grid(s_height, s_width, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int i = s_width / 2; i <= s_width; i += s_step) {
        cv::line(grid, {i, 0}, {i, s_height}, {200, 200, 200}, 1);
    }
    for (int i = s_width / 2; i >= 0; i -= s_step) {
        cv::line(grid, {i, 0}, {i, s_height}, {200, 200, 200}, 1);
    }
    for (int i = s_height; i >= 0 ; i -= s_step) {
        cv::line(grid, {0, i}, {s_width, i}, {200, 200, 200}, 1);
    }

    cv::line(grid, {0, s_height - s_step}, {s_width, s_height - s_step}, {0, 0, 200}, 1);
    cv::line(grid, {s_width / 2, 0}, {s_width / 2, s_height}, {0, 0, 200}, 1);

    for (auto &point: points) {
        cv::circle(grid, {int(point.x * grid_scale_factor) + s_width / 2 ,
                                      s_height - int(point.z * grid_scale_factor) - s_step}, 2, {0, 0, 255});
    }

    return grid;
}


int main() {
    cv::VideoCapture cap(source);
    cv::Mat frame;

    cv::namedWindow(slider_win_name);
    cv::createTrackbar("Low Hue", slider_win_name, &low_hue, 255);
    cv::createTrackbar("High Hue", slider_win_name, &high_hue, 255);
    cv::createTrackbar("Low Saturation", slider_win_name, &low_sat, 255);
    cv::createTrackbar("High Saturation", slider_win_name, &high_sat, 255);
    cv::createTrackbar("Low Value", slider_win_name, &low_val, 255);
    cv::createTrackbar("High Value", slider_win_name, &high_val, 255);

    cap.read(frame);

    while (true) {
        if (!cap.read(frame)) {
            cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
        }
//        frame = cv::imread("../lab6/videos/calib_1_0.jpg");  // Calibration image

        cv::Mat preprocessed = preprocess_frame(frame);
        auto points = frame2points(preprocessed);
        auto res = draw_points(points);

        cv::flip(preprocessed, preprocessed, 0);
        cv::imshow("res", res);
        cv::imshow("original", frame);
        cv::imshow("line view",preprocessed);

        if (cv::waitKey(50) == 27) break;
    }
    cv::destroyAllWindows();
    return 0;
}
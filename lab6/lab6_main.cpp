#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <algorithm>

const char* source = "../lab6/videos/2.avi";
const char* slider_win_name = "thresh_setup";


static int low_hue = 120;
static int high_hue = 255;
static int low_sat = 150;
static int high_sat = 255;
static int low_val = 0;
static int high_val = 255;

const float fovx = 74;
const float sensor_w = 7.5;  // Sensor width in mm to calculate focal distance
const float focal = sensor_w / 2 / tan(fovx / 2 / 180 * CV_PI);  // focal distance
const float cam_shift = 250;  // Distance from camera axis to laser plane


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
    cv::Mat nonzero;
    cv::findNonZero(frame, nonzero);
    std::vector<cv::Point3f> res;
    float pixel_size = sensor_w / (float)frame.cols;
    float center_x = (float)frame.cols / 2.0f;
    float center_y = (float)frame.rows / 2.0f;

    for (int idx = 0; idx < nonzero.total(); idx++) {
        cv::Point point = nonzero.at<cv::Point>(idx);

        float dx = ((float)point.x - center_x) * pixel_size;
        float dy = (center_y - (float)point.y) * pixel_size;

        float depth = cam_shift / dy * focal;
        float x = dx / dy * cam_shift;
        res.emplace_back(x, 0, depth);
    }
    return res;
}


cv::Mat displayDotsOnGrid(std::vector<cv::Point3f> points, float offset, int gridStep) {
    // Find min and max X and Z values
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

    // Create a grid
    int width = maxX - minX;
    int height = maxZ - minZ;
    cv::Mat grid(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int i = 0; i <= width; i += gridStep) {
        cv::line(grid, cv::Point(i, 0), cv::Point(i, height), cv::Scalar(200, 200, 200), 1);
    }
    for (int i = 0; i <= height; i += gridStep) {
        cv::line(grid, cv::Point(0, i), cv::Point(width, i), cv::Scalar(200, 200, 200), 1);
    }

    // Draw points on the grid
    for (const auto& point : points) {
        int x = (int)point.x / gridStep;
        int z = (int)point.z / gridStep;
        if (x >= 0 && x < width && z >= 0 && z < height) {
            grid.at<cv::Vec3b>(z, x) = cv::Vec3b(0, 0, 255); // Draw point in blue color
        }
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

        cv::Mat preprocessed = preprocess_frame(frame);
        auto points = frame2points(preprocessed);
        auto res = displayDotsOnGrid(points, 10, 15);

        cv::imshow("res", res);


        cv::imshow("original", frame);
        cv::imshow("line view",preprocessed);
        if (cv::waitKey(50) == 27) break;
    }
    cv::destroyAllWindows();
    return 0;
}
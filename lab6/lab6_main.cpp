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

const float fovx = 74;
const float sensor_w = 0.005;  // Sensor width in mm to calculate focal distance
const float focal = sensor_w / 2 / tan(fovx / 2);  // focal distance
const float cam_shift = 0.25;  // Distance from camera axis to laser plane


cv::Mat preprocess_frame(const cv::Mat &frame) {
    // Processes frame from camera to get 1px thick line
    cv::Mat res = frame.clone();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::inRange(frame, cv::Scalar(low_hue, low_sat, low_val),
                cv::Scalar(high_hue, high_sat, high_val), res);

    for (int col = 0; col < res.cols; col++) {  // Filter each column to have one averaged pixel
        auto column = res.col(col);
        int sum_row = 0;
        int count = 0;
        for (int row = 0; row < res.rows; row++) if (column.at<uchar>(row)) {
                sum_row += row;  // Calculate the sum of row indices having white pixels in the column
                count++;
        }
        if (count > 0) {
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
        float dy = ((float)point.y - center_y) * pixel_size;

        float depth = cam_shift * focal / dy;
        float x = depth * focal / dx;
        res.emplace_back(x, 0, depth);
    }
    return res;
}

void find_bounding_box(const std::vector<cv::Point3f>& points, cv::Point3f& minPoint, cv::Point3f& maxPoint) {
    minPoint = points[0];
    maxPoint = points[0];
    for (const auto& point : points) {
        if (point.x < minPoint.x) minPoint.x = point.x;
        if (point.z < minPoint.z) minPoint.z = point.z;

        if (point.x > maxPoint.x) maxPoint.x = point.x;
        if (point.z > maxPoint.z) maxPoint.z = point.z;
    }
}

void display_points(cv::Mat& img, const std::vector<cv::Point3f>& points, int gridSize = 10) {
    cv::Point3f max_point, min_point;
    find_bounding_box(points, min_point, max_point);


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
        cv::Mat res;
        display_points(res, points);

        cv::imshow("res", res);


        cv::imshow("original", frame);
        cv::imshow("line view",preprocessed);
        if (cv::waitKey(50) == 27) break;
    }
    cv::destroyAllWindows();
    return 0;
}
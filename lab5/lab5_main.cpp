#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "window.h"
#include "texture_render.h"

const int width = 1920;
const int height = 1080;
const float marker_length = 0.015;
const char* calibration_file_path = "../lab5/calibration.xml";

void read_calibration_params(cv::Mat& cam_mat, cv::Mat& dist_coeffs, const char* params_path) {
    cv::FileStorage fs(params_path, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open calibration file: " << params_path << std::endl;
        return;
    }
    fs["camera_matrix"] >> cam_mat;
    fs["distortion_coefficients"] >> dist_coeffs;
    fs.release();
}


int main() {
    Window window = {width, height, "lab5"};
    Renderer renderer;

    cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_250);

    cv::Mat cam_mat, dist_coeffs;
    read_calibration_params(cam_mat, dist_coeffs, calibration_file_path);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters parameters = cv::aruco::DetectorParameters();
    std::vector<int> markerIds;

    while (window.is_running()) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
            break;

//        cv::aruco::detectMarkers(frame, &dict, markerCorners, markerIds, &parameters, rejectedCandidates);


        renderer.update_texture(frame);
        renderer.Render();
        window.swap();
        glfwPollEvents();
    }

    cap.release();

    return 0;
}
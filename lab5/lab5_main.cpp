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
const std::string calibration_file_path = "../lab5/calibration.xml";
std::string detector_params_path = "../lab5/detector_params.yaml";

static void setup_detector_params(cv::aruco::DetectorParameters &params) {
    params.adaptiveThreshWinSizeMin = 18;
    params.adaptiveThreshWinSizeMax = 25;
    params.adaptiveThreshWinSizeStep = 1;
    params.adaptiveThreshConstant = 7;
    params.minMarkerPerimeterRate = 0.03;
    params.maxMarkerPerimeterRate = 4.0;
    params.polygonalApproxAccuracyRate = 0.05;
    params.minCornerDistanceRate = 10.0;
    params.minCornerDistanceRate = 10.0;
    params.minDistanceToBorder = 3;
    params.cornerRefinementWinSize = 5;
    params.cornerRefinementMaxIterations = 30;
    params.cornerRefinementMinAccuracy = 0.1;
    params.markerBorderBits = 1;
    params.perspectiveRemovePixelPerCell = 8;
    params.perspectiveRemoveIgnoredMarginPerCell = 0.13;
    params.maxErroneousBitsInBorderRate = 0.04;
}


void read_calibration_params(cv::Mat& cam_mat, cv::Mat& dist_coeffs, const std::string &params_path) {
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

    cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_250);
    cv::Mat cam_mat, dist_coeffs;
    read_calibration_params(cam_mat, dist_coeffs, calibration_file_path);

    Renderer renderer(width, height, cam_mat);
    renderer.update_position({0, 0, -5}, {0, 0, 0}, 3);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters parameters;
    setup_detector_params(parameters);
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
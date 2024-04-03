#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/aruco.hpp"
#include "window.h"
#include "texture_render.h"

const int width = 1920;
const int height = 1080;

const float marker_size = 0.065;

const std::string calibration_file_path = "../lab5/calibration.xml";


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

    cv::Mat cam_mat, dist_coeffs;
    read_calibration_params(cam_mat, dist_coeffs, calibration_file_path);
    Renderer renderer(width, height, cam_mat);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    cv::Mat objPoints(4, 1, CV_32FC3);
    objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-marker_size / 2.f, marker_size / 2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(marker_size / 2.f, marker_size / 2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(marker_size / 2.f, -marker_size / 2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-marker_size / 2.f, -marker_size / 2.f, 0);

    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);


    while (window.is_running()) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
            break;

        renderer.render_img(frame);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners, rejected;
        detector.detectMarkers(frame, corners, ids, rejected);

        if (!ids.empty()) {
            int nMarkers = static_cast<int>(corners.size());
            std::vector<cv::Vec3d> rvecs(nMarkers), tvecs(nMarkers);

            for (int i = 0; i < nMarkers; ++i) {
                cv::solvePnP(objPoints, corners.at(i), cam_mat, dist_coeffs, rvecs[i], tvecs[i]);
            }

            for (int i = 0; i < 1; ++i) {
                renderer.render_cube(tvecs[0], rvecs[0]);
            }

        }

        window.swap();
        glfwPollEvents();
    }

    cap.release();

    return 0;
}

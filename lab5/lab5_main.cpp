#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/aruco.hpp"
#include "window.h"
#include "glrenderer.h"

const int width = 1920;
const int height = 1080;

const float marker_size = 0.05;

const std::string calibration_file_path = "../lab5/calibration.xml";

std::vector<cv::Point3f> cv_cube_vertices = {cv::Point3f(-marker_size / 2, -marker_size / 2, 0),
                                             cv::Point3f(marker_size / 2, -marker_size / 2, 0),
                                             cv::Point3f(marker_size / 2, marker_size / 2, 0),
                                             cv::Point3f(-marker_size / 2, marker_size / 2, 0),
                                             cv::Point3f(-marker_size / 2, -marker_size / 2, marker_size),
                                             cv::Point3f(marker_size / 2, -marker_size / 2, marker_size),
                                             cv::Point3f(marker_size / 2, marker_size / 2, marker_size),
                                             cv::Point3f(-marker_size / 2, marker_size / 2, marker_size)};

void draw_cube(cv::Mat &frame, cv::Mat &cam_mat, cv::Mat &dist_coffs, cv::Vec3d &rvec, cv::Vec3d &tvec)
{
    std::vector<cv::Point2f> proj_points;
    cv::projectPoints(cv_cube_vertices, rvec, tvec, cam_mat, dist_coffs, proj_points);

    cv::line(frame, proj_points[0], proj_points[1], {0, 0, 255}, 4);
    cv::line(frame, proj_points[1], proj_points[2], {0, 255, 255}, 4);
    cv::line(frame, proj_points[2], proj_points[3], {0, 255, 0}, 4);
    cv::line(frame, proj_points[3], proj_points[0], {0, 255, 255}, 4);

    cv::line(frame, proj_points[4], proj_points[5], {255, 0, 0}, 4);
    cv::line(frame, proj_points[5], proj_points[6], {255, 0, 255}, 4);
    cv::line(frame, proj_points[6], proj_points[7], {255, 255, 0}, 4);
    cv::line(frame, proj_points[7], proj_points[4], {255, 255, 255}, 4);

    cv::line(frame, proj_points[0], proj_points[4], {0, 0, 125}, 4);
    cv::line(frame, proj_points[1], proj_points[5], {0, 125, 255}, 4);
    cv::line(frame, proj_points[2], proj_points[6], {125, 0, 255}, 4);
    cv::line(frame, proj_points[3], proj_points[7], {125, 125, 255}, 4);
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

    glRenderer renderer(width, height, cam_mat, dist_coeffs, marker_size);

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

        window.clear();

        renderer.draw_bg(frame);

        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners, rejected;
        detector.detectMarkers(frame, corners, ids, rejected);

        if (!ids.empty()) {
            int nMarkers = (int)corners.size();
            cv::Vec3d rvec, tvec;

            for (int i = 0; i < nMarkers; ++i) {
                cv::solvePnP(objPoints, corners.at(i), cam_mat, dist_coeffs, rvec, tvec);
                draw_cube(frame, cam_mat, dist_coeffs, rvec, tvec);
                renderer.draw_cube(tvec, rvec);
            }

        }
        cv::flip(frame, frame, 1);
        cv::imshow("lab5_cv", frame);
        if (cv::waitKey(50) == 27) break;
        window.swap();
        glfwPollEvents();
    }

    cap.release();

    return 0;
}

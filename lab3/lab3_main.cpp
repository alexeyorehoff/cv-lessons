#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"


cv::Mat isis_detect(cv::Mat img) {
    cv::Mat grayscale;
    cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);


}


int main() {
    std::cout << "Hello world" << std::endl;
    return 0;
}
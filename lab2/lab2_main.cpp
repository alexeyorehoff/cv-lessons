#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


int main() {
    cv::Mat original = cv::imread("../lab2/lenna.png");
    cv::imshow("original", original);
    cv::waitKey(0);
    return 0;
}

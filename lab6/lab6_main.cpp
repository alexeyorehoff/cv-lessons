#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

const char* source = "../lab6/videos/0.avi";

int main() {
    cv::VideoCapture cap(source);
    cv::Mat frame;
    cv::namedWindow("lab6");

    while (true) {
        if (!cap.read(frame)) {
            cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
        }
        cv::imshow("lab6",frame);
        if (cv::waitKey(50) == 27) break;
    }
    cv::destroyAllWindows();
    return 0;
}
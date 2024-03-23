#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "window.h"
#include "texture_render.h"
#include "shader.h"

const int width = 1920;
const int height = 1080;

int main() {
    Window window = {width, height, "lab5"};
    Renderer renderer;

    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video stream or file" << std::endl;
        return -1;
    }

    while (window.is_running()) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
            break;

        // Update texture with the new frame
        renderer.update_texture(frame);

        // Render the texture
        renderer.Render();

        // Swap buffers and poll events
        window.swap();
        glfwPollEvents();
    }

    cap.release();

    return 0;
}
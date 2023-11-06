#ifndef CV_LESSONS_APP_H
#define CV_LESSONS_APP_H

#include <opencv2/imgcodecs.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "sprite.h"

template<class ... Args>
class App {
    friend Sprite<Args ...>;
protected:
    cv::Size canvas_size;
    cv::Mat background_img;
    std::vector<Sprite<Args...>> sprites;

public:
    const char* title;

    App(int width, int height, const char *background_path, const char *title);

    cv::Mat render();
};


template<typename ... Args>
App<Args...>::App(int width, int height, const char *background_path, const char *title)
 : canvas_size(width, height), title(title) {
    using cv::Mat;

    Mat bg_img = cv::imread(background_path);
    cv::resize(bg_img, background_img, canvas_size);
}

template<typename ... Args>
cv::Mat App<Args ...>::render() {
    cv::Mat image = this->background_img.clone();
    for (Sprite<Args...> sprite: this->sprites) {
        cv::circle(image, cv::Point(sprite.x_center, sprite.y_center), 1,
                   cv::Scalar(0, 0, 255), 1);
        sprite.draw_on(image);
    }
    return image;
}

#endif //CV_LESSONS_APP_H

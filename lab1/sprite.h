#ifndef CV_LESSONS_SPRITE_H
#define CV_LESSONS_SPRITE_H

#include <utility>
#include <algorithm>
#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>


template<class ... Args>
class Sprite {
private:
    std::function<std::pair<int, int>(Args ...)> animation_func;
    cv::Mat texture;
    cv::Mat alpha_mask;
    int x_pos, y_pos;

public:
    int x_center;
    int y_center;
    const char *name;

    Sprite(const char *name, const char *texture_path);

    void set_animation(std::function<std::pair<int, int>(Args ...)> animation_func);

    void draw_on(cv::Mat background);

    void animate(Args ...);
};

template<typename ... Args>
Sprite<Args...>::Sprite(const char *name, const char *texture_path)
: name(name), x_center(0), y_center(0) {
    texture = cv::imread(texture_path, cv::IMREAD_UNCHANGED);
    cv::extractChannel(this->texture, alpha_mask, 3);
}

template<typename ... Args>
void Sprite<Args...>::set_animation(std::function<std::pair<int, int>(Args...)> func) {
    animation_func = func;
}

template<typename ... Args>
void Sprite<Args...>::draw_on(cv::Mat background) {
    cv::Rect roi_rect(x_pos, y_pos, texture.cols, texture.rows);
    roi_rect &= cv::Rect(0, 0, background.cols, background.rows);
    cv::Mat roi_region = background(roi_rect);
    cv::rectangle(roi_region, cv::Point(0, 0),
                              cv::Point(roi_region.cols - 1, roi_region.rows - 1),
                              cv::Scalar(0, 0, 255));
    auto texture_roi = cv::Rect(cv::Point(std::max(0, -x_pos), std::max(0, -y_pos)),
                                cv::Point(std::min(texture.cols, background.cols - x_pos),
                                              std::min(texture.rows, background.rows - y_pos)));

    cv::imshow("texture", texture(texture_roi));

}

template<typename ... Args>
void Sprite<Args...>::animate(Args... args) {
    auto pair = animation_func(args...);
    x_center = pair.first;
    y_center = pair.second;
    x_pos = x_center - texture.cols / 2;
    y_pos = y_center - texture.rows / 2;
}

#endif //CV_LESSONS_SPRITE_H
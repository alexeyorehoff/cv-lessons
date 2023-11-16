#ifndef CV_LESSONS_SPRITE_H
#define CV_LESSONS_SPRITE_H

#include <utility>
#include <algorithm>
#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>


template<class ... Args>
class Sprite {
protected:
    std::function<std::pair<int, int>(Args ...)> animation_func;
    cv::Mat texture;
    cv::Mat alpha_mask;
    int x_pos, y_pos;

public:
    float x_center;
    float y_center;
    const char *name;

    Sprite(const char *name, const char *texture_path);

    void set_animation(std::function<std::pair<int, int>(Args ...)> animation_func);

    void set_position(float pos_x, float pos_y);

    void draw_on(cv::Mat background);

    void animate(Args ...);
};

template<typename ... Args>
Sprite<Args...>::Sprite(const char *name, const char *texture_path)
: name(name), x_center(0), y_center(0) {
    cv::Mat image = cv::imread(texture_path, cv::IMREAD_UNCHANGED);
    cv::cvtColor(image, texture, cv::COLOR_BGRA2BGR);
    cv::extractChannel(image, alpha_mask, 3);
}

template<typename ... Args>
void Sprite<Args...>::set_animation(std::function<std::pair<int, int>(Args...)> func) {
    animation_func = func;
}

template<typename ... Args>
void Sprite<Args...>::draw_on(cv::Mat background) {
    cv::Rect bg_roi(x_pos, y_pos, texture.cols, texture.rows);
    bg_roi &= cv::Rect(0, 0, background.cols, background.rows);
    cv::Rect texture_roi = cv::Rect(std::max(0, -x_pos), std::max(0, -y_pos), bg_roi.width, bg_roi.height);
    cv::copyTo(texture(texture_roi), background(bg_roi), alpha_mask(texture_roi));
}

template<typename ... Args>
void Sprite<Args ...>::set_position(float pos_x, float pos_y) {
    x_center = pos_x;
    y_center = pos_y;
    x_pos = (int)x_center - texture.cols / 2;
    y_pos = (int)y_center - texture.rows / 2;
}

template<typename ... Args>
void Sprite<Args...>::animate(Args... args) {
    auto pair = animation_func(args...);
    this->set_position(pair.first, pair.second);
}

#endif //CV_LESSONS_SPRITE_H
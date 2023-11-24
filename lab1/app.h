#ifndef CV_LESSONS_APP_H
#define CV_LESSONS_APP_H

#include <opencv2/imgcodecs.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "sprite.h"

template<class T>
class App {
    friend T;
protected:
    cv::Size canvas_size;
    cv::Mat background_img;
    std::vector<T> sprites;

public:
    const char* title;

    App(int width, int height, const char *background_path, const char *title);

    void add_sprite(T sprite);

    virtual cv::Mat render();
};


template<typename T>
App<T>::App(int width, int height, const char *background_path, const char *title)
 : canvas_size(width, height), title(title) {
    cv::Mat bg_img = cv::imread(background_path);
    cv::resize(bg_img, background_img, canvas_size);
}

template<typename T>
void App<T>::add_sprite(T sprite) {
    sprites.push_back(sprite);
}

template<typename T>
cv::Mat App<T>::render() {
    cv::Mat image = this->background_img.clone();
    for (T sprite: this->sprites) {
        if (sprite.fits_in(canvas_size)) {
            sprite.draw_on(image);
        }
    }
    return image;
}

#endif //CV_LESSONS_APP_H

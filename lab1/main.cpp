#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "sprite.h"
#include "app.h"

static long int x_position = 0;

class SimpleSpriteApp : public App<int> {
public:
    int reverse_dir = 1;

    SimpleSpriteApp(int width, int height, const char *background_path, const char *title, const Sprite<int>& robot)
    : App<int>(width, height, background_path, title) {
        sprites.push_back(robot);
    }

    cv::Mat render() {
        x_position += reverse_dir;
        if ((reverse_dir > 0 && x_position > background_img.cols) || (reverse_dir < 0 && x_position < 1)) {
            reverse_dir *= -1;
        }

        for (Sprite<int> &sprite: sprites) {
            sprite.animate(x_position);
//            cv::circle(background_img, cv::Point(sprite.x_center, sprite.y_center), 1,
//                       cv::Scalar(0, 0, 255), 1);
        }
        return App::render();
    }
};


int main() {
    Sprite<int> robot("robot", "../lab1/robot.png");
    robot.set_animation([](int time){return std::pair<int, int>(time, int(500 + 250 * sin((float)time / 100)));});

    SimpleSpriteApp app(1000, 1000, "../lab1/background.jpg", "test_project", robot);

    cv::imshow(app.title, app.render());
    cv::waitKey(0);

    while (true) {
        cv::imshow(app.title, app.render());
        int c = cv::waitKey(10);
        if (c == 27) {
            break;
        }
    }
    return 0;
}

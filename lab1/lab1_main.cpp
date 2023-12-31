#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "sprite.h"
#include "app.h"

static int x_position = 0;

class SimpleSpriteApp : public App<Sprite<int>> {
public:
    int speed = 1;

    SimpleSpriteApp(int width, int height, const char *background_path, const char *title)
    : App<Sprite<int>>(width, height, background_path, title) {}

    cv::Mat render() override {
        x_position += speed;
        if ((speed > 0 && x_position > background_img.cols) || (speed < 0 && x_position < 1)) {
            speed *= -1;
        }
        for (Sprite<int> &sprite: sprites) {
            sprite.animate(x_position);
            cv::circle(background_img, cv::Point((int)sprite.x_center, (int)sprite.y_center),
                       1,cv::Scalar(0, 0, 255), 1);
        }

        return App::render();
    }
};


int main() {
    SimpleSpriteApp app(640, 480, "../lab1/background.jpg", "test_project");

    Sprite<int> robot1("robot1", "../lab1/robot.png");
    Sprite<int> robot2("robot2", "../lab1/robot.png");
    robot1.set_animation([](int time){return cv::Point2i(time, int(240 + 350 * sin((float)time / 100)));});
    robot2.set_animation([](int time){return cv::Point2i (time, int(200 + 150 * cos((float)time / 100)));});

    app.add_sprite(robot1);
    app.add_sprite(robot2);

    while (true) {
        auto image = app.render();
        cv::imshow(app.title, image);

        if (x_position == 320) {
            cv::imwrite("test.jpg", image);
        }

        int c = cv::waitKey(10);
        if (c == 27) {
            break;
        }
    }
    return 0;
}

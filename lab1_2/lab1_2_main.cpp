#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "app.h"
#include "sprite.h"


const double G_CONST = 100;

template <typename T> T signed_square(T val) {
    int sign = (val < 0) ? -1 : 1;
    return sign * val * val;
}

class Body : public Sprite<double> {  // Класс тела в задаче N-тел. В уравнение для расчета позиции
                                                 // задаются компоненты скорости и промежуток времени dt
public:
    int mass;
    double vx, vy;

    Body(const char* name, const char* texture_path, int mass, int pos_x, int pos_y, float vx, float vy)
    : mass(mass), Sprite<double>(name, texture_path), vx(vx), vy(vy) {
        set_position(pos_x, pos_y);
    }
};

class NBodyApp : public App<Body> {
public:
    NBodyApp(int width, int height, const char *background_path, const char *title)
    : App<Body>(width, height, background_path, title) {}

    cv::Mat render() {
        float dt = 1;
        for (auto& body : sprites) {
            double ax = 0.0;
            double ay = 0.0;

            for (const auto& other_body: sprites) {
                if (&other_body == &body) continue;
                int dx = other_body.x_center - body.x_center;
                int dy = other_body.y_center - body.y_center;
                double r = std::sqrt(dx * dx + dy * dy);
                double r_accel = other_body.mass / (r * r);
                ax += r_accel * dx / r;
                ay += r_accel * dy / r;
            }
            body.vx += ax * G_CONST * dt;
            body.vy += ay * G_CONST * dt;
        }
        for (auto& body: sprites) {
            body.set_position(body.x_center + int(body.vx * dt),
                              body.y_center + int(body.vy * dt));
        }
        return App::render();
    }
};


int main() {
    // Body sun("sun", "../lab1_2/sun.png", 100, 500, 500, 0, 0);
    Body planet1("planet1", "../lab1_2/planet.png", 100, 500, 500, 0, 0);
    Body planet2("planet2", "../lab1_2/planet.png", 1, 100, 500, 0, 3);
//    Body planet3("planet3", "../lab1_2/planet.png", 100, 800, 500, 0, 0);
    NBodyApp app(1000, 1000, "../lab1_2/background.jpg", "test");

    app.add_sprite(planet1);
    app.add_sprite(planet2);
//    app.add_sprite(planet3);

    while (true) {
        cv::imshow(app.title, app.render());
        int c = cv::waitKey(10);
        if (c == 27) {
            break;
        }
    }

    return 0;
}
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "app.h"
#include "sprite.h"


const float G_CONST = 5;

class Body : public Sprite<double> {
public:
    float mass, vx, vy;

    Body(const char* name, const char* texture_path, float mass, int pos_x, int pos_y, float vx, float vy)
    : mass(mass), Sprite<double>(name, texture_path), vx(vx), vy(vy) {
        set_position((float)pos_x, (float)pos_y);
    }
};

class NBodyApp : public App<Body> {
public:
    NBodyApp(int width, int height, const char *background_path, const char *title)
    : App<Body>(width, height, background_path, title) {}

    cv::Mat render() override {
        float dt = 0.5;
        for (auto& body : sprites) {
            float ax = 0.0;
            float ay = 0.0;

            for (const auto& other_body: sprites) {
                if (&other_body == &body) continue;
                float dx = other_body.x_center - body.x_center;
                float dy = other_body.y_center - body.y_center;
                float r = std::sqrt(dx * dx + dy * dy);
                float r_accel = (float)other_body.mass / (r * r);
                ax += r_accel * dx / r;
                ay += r_accel * dy / r;
            }
            body.vx += ax * G_CONST * dt;
            body.vy += ay * G_CONST * dt;
        }
        for (auto& body: sprites) {
            body.set_position(body.x_center + body.vx * dt,
                              body.y_center + body.vy * dt);
        }
        return App::render();
    }
};


int main() {
    // Body sun("sun", "../lab1_2/sun.png", 100, 500, 500, 0, 0);
    Body planet1("planet1", "../lab1/images/planet.png", 5000, 100, 500, 0.5, 0);
    Body planet2("planet2", "../lab1/images/planet.png", 75, 100, 700, 12, 0);
    Body planet3("planet3", "../lab1/images/planet.png", 50, 100, 300, 10, 0);
    Body planet4("planet4", "../lab1/images/planet.png", 100, 400, 500, 0, 10);

    NBodyApp app(1920, 1080, "../lab1/images/background2.png", "test");

    app.add_sprite(planet1);
    app.add_sprite(planet2);
    app.add_sprite(planet3);
    app.add_sprite(planet4);

    while (true) {
        cv::imshow(app.title, app.render());
        int c = cv::waitKey(10);
        if (c == 27) {
            break;
        }
    }

    return 0;
}
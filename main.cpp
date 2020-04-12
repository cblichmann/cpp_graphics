//
// Created by federico on 09/04/2020.
//

#include <iostream>
#include <cmath>
#include <assert.h>
#include <fstream>
#include "Utils/TimeGeometry/TimeVector3.h"
#include "Utils/TimeGeometry/TimeUnitVector3.h"
#include "Utils/TimeGeometry/TimeVector2.h"
#include "Utils/Geometry/Basis.h"
#include "Utils/Geometry/ReferenceFrame.h"
#include "Scene.h"
#include "Object/Ball.h"
#include "Camera/SimpleCamera.h"
#include "Object/Object2D.h"
#include "Surface/Circle.h"
#include "Surface/Polygon.h"
#include "Object/Cube.h"
#include "Light/SimpleLight.h"

void test_ref_frame() {

    Vector3 v(rand()%10000, rand()%10000, rand()%10000);
    v = v.scale(100);

    Basis basis(
            TimeUnitVector3(3.0/5, 4.0/5, 0),
            TimeUnitVector3(-4.0/5, 3.0/5, 0),
            TimeUnitVector3(0, 0, 1)
    );

    ReferenceFrame referenceFrame(
            TimeVector3(-3.9, 2, 4.2),
            basis
    );

    Vector3 vv = referenceFrame.to_ref_frame(v, 0);
    Vector3 vvv = referenceFrame.from_ref_frame(vv, 0);

    assert(v.equals(vvv));

}

void print_picture(const std::string& picture, const std::string& file = (std::string) "pic.ppm", bool open = true) {

    std::ofstream out(file);
    out<<picture<<std::endl;

    if (open) {
        std::string command = "xdg-open " + file;
        system(command.c_str());
    }

}

void video(Camera & c, int height, int width, const Scene& a) {

    int FRAMES = 20;
    int TIME = 5;

    for (int i=0; i<FRAMES; i++) {

        std::cout << (i + 1) << "/" << FRAMES << "\r";
        std::cout.flush();

        std::string file = "im" +
                           (std::string) ((i < 10) ? "0" : "") +
                           (std::string) ((i < 100) ? "0" : "") +
                           std::to_string(i) +
                           ".ppm";

        print_picture(c.picture(height, width, (double) i / FRAMES), file, false);

    }

    std::string command = "ffmpeg -y -v 16 -framerate " + std::to_string(FRAMES / TIME) + " -pattern_type glob -i \"im*.ppm\" video.avi; rm im*.ppm; xdg-open video.avi";
    system(command.c_str());

};

SimpleCamera test_camera(Scene s) {

    double distance = 500;

    TimeVector3 position(
    TimeFunction([=](double t){ return distance * sin(M_PI * t);}),
            TimeFunction([=](double t){ return distance * t;}),
            TimeFunction([=](double t){ return distance * cos(M_PI * t);})
    );

    TimeUnitVector3 base_x(
    TimeFunction([=](double t){ return cos(M_PI * t);}),
            TimeFunction([=](double t){ return 0;}),
            TimeFunction([=](double t){ return -sin(M_PI * t);})
    );

    TimeUnitVector3 base_y(
    TimeFunction([=](double t){ return - t * sin(M_PI * t) / sqrt(t*t+1);}),
            TimeFunction([=](double t){ return 1.0 /sqrt(t*t+1);}),
            TimeFunction([=](double t){ return - t * cos(M_PI * t) / sqrt(t*t+1);})
    );

    TimeUnitVector3 base_z(
    TimeFunction([=](double t){ return sin(M_PI * t) / sqrt(t*t+1);}),
            TimeFunction([=](double t){ return t / sqrt(t*t+1);}),
            TimeFunction([=](double t){ return cos(M_PI * t) / sqrt(t*t+1);})
    );

    return SimpleCamera(ReferenceFrame(position, Basis(base_x, base_y, base_z)), s);
}

int main() {

    //TODO: do unit testing

    Aspect aspect1(Color(255, 209, 241));
    Aspect aspect2(Color(226, 209, 255));
    Aspect aspect3(Color(209, 231, 255));
    Aspect aspect4(Color(181, 2, 88));
    Aspect aspect5(Color(154, 255, 71));

    test_ref_frame();

    Scene s;

    Basis b(
            TimeUnitVector3(1, 0, 0),
            TimeUnitVector3(0, 0, 1),
            TimeUnitVector3(0, -1, 0)
            );

    std::vector<Vector2> points ={
            Vector2(70, 70),
            Vector2(-70, 70),
            Vector2(-70, -70),
            Vector2(70, -70)
    };

    s.add(new Ball (ReferenceFrame(TimeVector3(0, 10, 0), Basis()), aspect5, 10));
    s.add(new Ball (ReferenceFrame(TimeVector3(0, 30, -50), Basis()), aspect4, 30));
    s.add(new Object2D(ReferenceFrame(TimeVector3(0, 60, -50), b), new Circle(ReferenceFrame(TimeVector3(0, 0, 0), Basis()), aspect5, 20)));
    s.add(new Object2D(ReferenceFrame(TimeVector3(0, 0, 0), b), new Polygon(ReferenceFrame(TimeVector3(0, 0, 0), Basis()), aspect2, points)));
    s.add(new Cube(ReferenceFrame(TimeVector3(0, 20, 40), Basis()), aspect3, 40));
    s.add(new SimpleLight(ReferenceFrame(TimeVector3(0, 100, 0), Basis()), 1));

    SimpleCamera c = test_camera(s);

    print_picture(c.picture(1000, 1000, 0.3));
    //video(c, 200, 200, s);

}

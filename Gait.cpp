//
// Created by Bogdan on 4/27/2025.
//

#include "include/Gait.h"

// Gait.cpp
#include <iostream>

#include "include/Kinematics.h"
#include "include/Servo.h"
#include <thread>

void animationMRLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad) {
    double S = 60;
    double A = 30;
    double T = 60;

    int numberOfPoint = 20;

    struct Point p1;
    p1.x = 0;
    p1.y = 0;
    p1.z = 0;

    struct Point p2;
    p2.x = T / 2;
    p2.y = 0;
    p2.z = S / 2;

    struct Point p3;
    p3.x = T;
    p3.y = 0;
    p3.z = 0;

    //smooth transition between P1 and P2
    std::vector<float> x_path = linespace(p1.x, p2.x, numberOfPoint);
    std::vector<float> y_path = linespace(p1.y, p2.y, numberOfPoint);
    std::vector<float> z_path = linespace(p1.z, p2.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x_path[i];
        p.y = y_path[i];
        p.z = z_path[i];

        std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x2_path = linespace(p2.x, p3.x, numberOfPoint);
    std::vector<float> y2_path = linespace(p2.y, p3.y, numberOfPoint);
    std::vector<float> z2_path = linespace(p2.z, p3.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x2_path[i];
        p.y = y2_path[i];
        p.z = z2_path[i];

        std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x3_path = linespace(p3.x, p1.x, numberOfPoint);
    std::vector<float> y3_path = linespace(p3.y, p1.y, numberOfPoint);
    std::vector<float> z3_path = linespace(p3.z, p1.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x3_path[i];
        p.y = y3_path[i];
        p.z = z3_path[i];

        std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }
}

void animationMLLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad) {
    double S = 60;
    double A = 30;
    double T = 60;

    int numberOfPoint = 20;

    struct Point p1;
    p1.x = 0;
    p1.y = 0;
    p1.z = 0;

    struct Point p2;
    p2.x = T / 2;
    p2.y = 0;
    p2.z = -S / 2;

    struct Point p3;
    p3.x = T;
    p3.y = 0;
    p3.z = 0;

    //smooth transition between P1 and P2
    std::vector<float> x_path = linespace(p1.x, p2.x, numberOfPoint);
    std::vector<float> y_path = linespace(p1.y, p2.y, numberOfPoint);
    std::vector<float> z_path = linespace(p1.z, p2.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x_path[i];
        p.y = y_path[i];
        p.z = z_path[i];

        std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x2_path = linespace(p2.x, p3.x, numberOfPoint);
    std::vector<float> y2_path = linespace(p2.y, p3.y, numberOfPoint);
    std::vector<float> z2_path = linespace(p2.z, p3.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x2_path[i];
        p.y = y2_path[i];
        p.z = z2_path[i];

        std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x3_path = linespace(p3.x, p1.x, numberOfPoint);
    std::vector<float> y3_path = linespace(p3.y, p1.y, numberOfPoint);
    std::vector<float> z3_path = linespace(p3.z, p1.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x3_path[i];
        p.y = y3_path[i];
        p.z = z3_path[i];

        std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }
}

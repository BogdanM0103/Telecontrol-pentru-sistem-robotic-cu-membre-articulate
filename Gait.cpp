//
// Created by Bogdan on 4/27/2025.
//

#include "include/Gait.h"

// Gait.cpp
#include <iostream>

#include "include/Kinematics.h"
#include "include/Servo.h"
#include <thread>

// generate one cycle of a “triangular” foot stroke
//   S = stroke height, T = stride length, n = points per segment,
//   liftZ = +S/2 to lift up, –S/2 to dip down
std::vector<Point> makeFootCycle(double S, double T, int n, double liftZ) {
    std::vector<Point> cycle;
    cycle.reserve(n * 3);

    // segment 1: back → up
    auto xs1 = linespace(-T/2,   0, n);
    auto zs1 = linespace(   0, liftZ, n);
    for (int i = 0; i < n; ++i)
        cycle.push_back({ xs1[i], 0.0, zs1[i] });

    // segment 2: up → forward
    auto xs2 = linespace(   0, T/2, n);
    auto zs2 = linespace(liftZ,   0, n);
    for (int i = 0; i < n; ++i)
        cycle.push_back({ xs2[i], 0.0, zs2[i] });

    // segment 3: forward → back (flat)
    auto xs3 = linespace( T/2, -T/2, n);
    for (int i = 0; i < n; ++i)
        cycle.push_back({ xs3[i], 0.0, 0.0 });

    return cycle;
}

void animateLeg(int coxaPin,
                int femurPin,
                int tibiaPin,
                double angleRad,   // how much to spin around Z
                double S,          // stroke height
                double T,          // stride length
                int    n,          // points per segment
                double liftZ)      // +S/2 for lift, –S/2 for dip
{
    auto cycle = makeFootCycle(S, T, n, liftZ);

    for (auto const& localP : cycle) {
        // rotate the (x,0,z) stroke around Z by angleRad
        Point rotated = rotateXY(localP, angleRad);

        // compute IK & drive servos
        Angles a = posToAngle(rotated);
        // moveServo(coxaPin,  a.J1);
        // moveServo(femurPin, a.J2);
        // moveServo(tibiaPin, a.J3);
        moveServo(coxaPin, femurPin, tibiaPin, a.J1, a.J2, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void animationMRLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad) {
    // MR lifts up
    animateLeg(coxaPin, femurPin, tibiaPin,
               angleRad,    // e.g. +30° in radians
               /*S=*/60, /*T=*/60, /*n=*/10,
               /*liftZ=*/+60.0/2);
}

void animationMLLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad) {
    // ML dips down (negative liftZ)
    animateLeg(coxaPin, femurPin, tibiaPin,
               angleRad,
               /*S=*/60, /*T=*/60, /*n=*/10,
               /*liftZ=*/-60.0/2);
}
//
// Created by Bogdan on 4/27/2025.
//

#include "include/Gait.h"

#include "include/Kinematics.h"
#include "include/Servo.h"
#include <thread>

inline Point bezierCubic(double t,
                         const Point &P0,
                         const Point &P1,
                         const Point &P2,
                         const Point &P3)
{
    double u  = 1.0 - t;
    double u2 = u * u;
    double u3 = u2 * u;
    double t2 = t * t;
    double t3 = t2 * t;

    Point B;
    B.x = u3 * P0.x
        + 3.0 * u2 * t * P1.x
        + 3.0 * u  * t2 * P2.x
        +      t3 * P3.x;

    B.y = u3 * P0.y
        + 3.0 * u2 * t * P1.y
        + 3.0 * u  * t2 * P2.y
        +      t3 * P3.y;

    B.z = u3 * P0.z
        + 3.0 * u2 * t * P1.z
        + 3.0 * u  * t2 * P2.z
        +      t3 * P3.z;

    return B;
}

// generate one cycle of a “triangular” foot stroke
//   S = stroke height, T = stride length, n = points per segment,
//   liftZ = +S/2 to lift up, –S/2 to dip down
std::vector<Point> makeFootCycle(double S, double T, int n, double liftZ) {
    std::vector<Point> cycle;
    cycle.reserve(n * 2 + n);  // swing + stance

    // Define control points for swing phase
    Point P0 = { -T/2, 0.0, 0.0 };           // start (back)
    Point P1 = { -T/4, 0.0, liftZ };         // control point 1 (up)
    Point P2 = {  T/4, 0.0, liftZ };         // control point 2 (up)
    Point P3 = {  T/2, 0.0, 0.0 };           // end (front)

    for (int i = 0; i < n * 2; ++i) {
        double t = static_cast<double>(i) / (n * 2 - 1);
        cycle.push_back(bezierCubic(t, P0, P1, P2, P3));
    }

    // Stance phase: forward → back (flat)
    auto xs3 = linespace(T/2, -T/2, n);
    for (int i = 0; i < n; ++i)
        cycle.push_back({ xs3[i], 0.0, 0.0 });

    return cycle;
}
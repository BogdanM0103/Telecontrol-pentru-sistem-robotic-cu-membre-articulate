//
// Created by Bogdan on 4/27/2025.
//

#include "include/Kinematics.h"

#include <algorithm>
#include <cmath>

// Generates `num_points` equally spaced values from `start` to `end` (inclusive).
std::vector<float> linespace(float start, float end, int num_points) {
    std::vector<float> values;
    if (num_points <= 1) {
        values.push_back(start);
        return values;
    }
    values.reserve(num_points);
    float step = (end - start) / static_cast<float>(num_points - 1);
    for (int i = 0; i < num_points; ++i) {
        values.push_back(start + step * i);
    }
    return values;
}

struct Angles posToAngle(const struct Point& p) {
    double x = p.x;
    double y = p.y;
    double z = p.z;

    // Apply rest offsets
    y += YRest;
    z += ZRest;

    // Correctly handle quadrant: atan2 avoids flipping
    double J1 = atan2(x, y) * (180.0 / PI);

    double H = sqrt(x * x + y * y);
    double L = sqrt(H * H + z * z);

    double cosJ3 = ((J2L * J2L) + (J3L * J3L) - (L * L)) / (2.0 * J2L * J3L);
    cosJ3 = std::clamp(cosJ3, -1.0, 1.0); // prevent domain errors
    double J3 = acos(cosJ3) * (180.0 / PI);

    double cosB = ((L * L) + (J2L * J2L) - (J3L * J3L)) / (2.0 * L * J2L);
    cosB = std::clamp(cosB, -1.0, 1.0);
    double B = acos(cosB) * (180.0 / PI);

    double A = atan2(z, H) * (180.0 / PI);
    double J2 = B + A;

    struct Angles a;
    a.J1 = 90.0 - J1;  // Turning toward X axis
    a.J2 = 90.0 + J2;  // Up/down
    a.J3 = J3 + 2.0;   // Joint compensation offset
    return a;
}

Point rotateXY(const Point& p, double angleRad) {
    double cosA = std::cos(angleRad);
    double sinA = std::sin(angleRad);
    return Point{
        // new X =  x*cosθ − y*sinθ
        p.x * cosA - p.y * sinA,
        // new Y =  x*sinθ + y*cosθ
        p.x * sinA + p.y * cosA,
        // Z unchanged
        p.z
    };
}
//
// Created by Bogdan on 4/27/2025.
//

#pragma once

struct Point {
    double x, y, z;
};

struct Angles {
    double J1, J2, J3;
};

// limb geometry
constexpr double J2L   = 58.0;
constexpr double J3L   = 138.0;
constexpr double YRest = 58.0;
constexpr double ZRest = -138.0;
constexpr double PI    = 3.141592653589793;

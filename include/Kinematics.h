//
// Created by Bogdan on 4/27/2025.
//

// Kinematics.h
#pragma once
#include <vector>

#include "Types.h"

Angles posToAngle(const Point& p);

Point rotateXY(const Point& p, double angleRad);

std::vector<float> linespace(float start, float end, int num_points);



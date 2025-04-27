//
// Created by Bogdan on 4/27/2025.
//

// Kinematics.h
#pragma once
#include "Types.h"

// Transforms global (robot-centered) foot position into leg-local coords
Point globalToLocal(const Point& global, double baseX, double baseY);
Point localToGlobal(const Point& local,  double baseX, double baseY);

// Given a local foot Point, compute joint angles
Angles posToAngle(const Point& p);

// Optional utility
Point rotateXY(const Point& p, double angleRad);

//
// Created by Bogdan on 4/27/2025.
//

// Gait.h
#pragma once
#include <vector>

#include "Types.h"

void animationMLLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad);
void animationMRLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad);
void testPose    (int coxaPin, int femurPin, int tibiaPin);

std::vector<Point> makeFootCycle(double S, double T, int n, double liftZ);
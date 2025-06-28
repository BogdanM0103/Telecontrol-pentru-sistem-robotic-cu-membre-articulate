//
// Created by Bogdan on 4/27/2025.
//

// Servo.h
#pragma once
#include <initializer_list>

void moveServo(int channel, int degrees);

int degreesToPulseWidth(int degrees);

void moveServo(int channel1, int channel2, int channel3, double degrees1, double degrees2, double degrees3);

void disableServo(int channel);

void moveFirstTripod(double angleDeg);

void moveSecondTripod(double angleDeg);

void rotateLeftInfinite();

void rotateFirstTripodInPlaceLeft();

void rotateSecondTripodInPlaceLeft();

void rotateFirstTripodInPlaceRight();

void rotateSecondTripodInPlaceRight();

void unstiffenIdleCoxae(const std::initializer_list<int>& channels);

void freezeAllServos();
//
// Created by Bogdan on 4/27/2025.
//

// Servo.h
#pragma once

// channel 0–31, degrees 0–180
void moveServo(int channel, int degrees);

// if you need direct pulse-width mapping
int degreesToPulseWidth(int degrees);

void moveServo(int channel1, int channel2, int channel3, double degrees1, double degrees2, double degrees3);

void moveFirstTripod();

void moveSecondTripod();

void rotateLeftInfinite();

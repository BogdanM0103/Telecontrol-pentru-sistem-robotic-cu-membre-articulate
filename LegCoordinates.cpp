//
// Created by Bogdan on 4/27/2025.
//

#include "LegCoordinates.h"

// LegCoordinates.cpp
#include "LegCoordinates.h"

// define your globals
Point HL, ML, LL, HR, MR, LR;

void assignCoordinatesCoxa() {
    HL = {-70,  121.24, 0};
    ML = {-140,   0.00, 0};
    LL = {-70, -121.24, 0};
    HR = { 70,  121.24, 0};
    MR = {140,    0.00, 0};
    LR = { 70, -121.24, 0};  // fixed typo from original: should be +70
}
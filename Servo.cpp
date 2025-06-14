//
// Created by Bogdan on 4/27/2025.
//

#include "include/Servo.h"

#include <cstring>
#include <iostream>
#include <math.h>
#include <thread>
#include <unistd.h>

#include "LegCoordinates.h"
#include "include/Gait.h"
#include "include/Kinematics.h"
#include "include/Serial.h"

constexpr double DEG2RAD = M_PI/180.0;

int degreesToPulseWidth(int degrees) {
    if (degrees < 0) degrees = 0;
    if (degrees > 180) degrees = 180;

    // Map 0° -> 500 µs, 180° -> 2500 µs
    return 500 + (degrees * (2000.0 / 180.0));
}

void moveServo(int channel, int degrees) {
    if (serialPortFD == -1) {
        std::cerr << "Serial port not open." << std::endl;
        return;
    }

    if (channel < 0 || channel > 31) {
        std::cerr << "Invalid channel number." << std::endl;
        return;
    }


    //Check Coxa degrees
    if (channel == 0 || channel == 4 || channel == 8 || channel == 16 || channel == 20 || channel == 24) {
        if (degrees < 10 || degrees > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Femur degrees
    if (channel == 1 || channel == 5 || channel == 9 || channel == 17 || channel == 21 || channel == 25) {
        if (degrees < 55 || degrees > 130) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Tibia degrees
    if (channel == 2 || channel == 6 || channel == 10 || channel == 18 || channel == 22 || channel == 26) {
        if (degrees < 60 || degrees > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }


    int pulseWidth = 500 + (int)(degrees * (2000.0 / 180.0)); // map 0–180° to 500–2500μs
    char command[32];
    snprintf(command, sizeof(command), "#%dP%dT100\r", channel, pulseWidth);

    write(serialPortFD, command, strlen(command));
}

void moveServo(int channel1, int channel2, int channel3, double degrees1, double degrees2, double degrees3) {
    if (serialPortFD == -1) {
        std::cerr << "Serial port not open." << std::endl;
        return;
    }

    if (channel1 < 0 || channel1 > 31) {
        std::cerr << "Invalid channel number." << std::endl;
        return;
    }


    //Check Coxa degrees
    if (channel1 == 0 || channel1 == 4 || channel1 == 8 || channel1 == 16 || channel1 == 20 || channel1 == 24) {
        if (degrees1 < 10 || degrees1 > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Femur degrees
    if (channel1 == 1 || channel1 == 5 || channel1 == 9 || channel1 == 17 || channel1 == 21 || channel1 == 25) {
        if (degrees1 < 55 || degrees1 > 130) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Tibia degrees
    if (channel1 == 2 || channel1 == 6 || channel1 == 10 || channel1 == 18 || channel1 == 22 || channel1 == 26) {
        if (degrees1 < 60 || degrees1 > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    // channel 3
    if (serialPortFD == -1) {
        std::cerr << "Serial port not open." << std::endl;
        return;
    }

    if (channel3 < 0 || channel3 > 31) {
        std::cerr << "Invalid channel number." << std::endl;
        return;
    }


    //Check Coxa degrees
    if (channel3 == 0 || channel3 == 4 || channel3 == 8 || channel3 == 16 || channel3 == 20 || channel3 == 24) {
        if (degrees3 < 10 || degrees3 > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Femur degrees
    if (channel3 == 1 || channel3 == 5 || channel3 == 9 || channel3 == 17 || channel3 == 21 || channel3 == 25) {
        if (degrees3 < 55 || degrees3 > 130) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Tibia degrees
    if (channel3 == 2 || channel3 == 6 || channel3 == 10 || channel3 == 18 || channel3 == 22 || channel3 == 26) {
        if (degrees3 < 60 || degrees3 > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    if (serialPortFD == -1) {
        std::cerr << "Serial port not open." << std::endl;
        return;
    }

    if (channel2 < 0 || channel2 > 31) {
        std::cerr << "Invalid channel number." << std::endl;
        return;
    }


    //Check Coxa degrees
    if (channel2 == 0 || channel2 == 4 || channel2 == 8 || channel2 == 16 || channel2 == 20 || channel2 == 24) {
        if (degrees2 < 10 || degrees2 > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Femur degrees
    if (channel2 == 1 || channel2 == 5 || channel2 == 9 || channel2 == 17 || channel2 == 21 || channel2 == 25) {
        if (degrees2 < 55 || degrees2 > 130) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Tibia degrees
    if (channel2 == 2 || channel2 == 6 || channel2 == 10 || channel2 == 18 || channel2 == 22 || channel2 == 26) {
        if (degrees2 < 60 || degrees2 > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }


    int pulseWidth1 = 500 + (int)(degrees1 * (2000.0 / 180.0)); // map 0–180° to 500–2500μs
    int pulseWidth2 = 500 + (int)(degrees2 * (2000.0 / 180.0)); // map 0–180° to 500–2500μs
    int pulseWidth3 = 500 + (int)(degrees3 * (2000.0 / 180.0)); // map 0–180° to 500–2500μs
    char command[32];
    snprintf(command, sizeof(command), "#%dP%d#%dP%d#%dP%dT100\r", channel1, pulseWidth1, channel2, pulseWidth2, channel3, pulseWidth3);

    write(serialPortFD, command, strlen(command));
}

void moveFirstTripod(double angleDeg) {
    assignCoordinatesCoxa();


    // stroke parameters
    const double S = 60.0, T = 60.0;
    const int    n = 10;             // points per segment
    const int    cycleMs = 1000;     // total cycle duration (1s)
    const int    frameDelay = cycleMs / (n * 3);

    // convert your rotation‐offset into radians
    double offsetRad = angleDeg * DEG2RAD;

    // precompute two raw foot-paths (Y=0):
    //  - back/dip for ML legs (liftZ negative)
    //  - front/lift for MR leg (liftZ positive)
    auto pathML = makeFootCycle(S, T, n, -S/2.0);
    auto pathMR = makeFootCycle(S, T, n, +S/2.0);

    for (size_t i = 0; i < pathML.size(); ++i) {
        // Leg A (channels 0,1,2)  → ML at –45°
        Point pA = rotateXY(pathML[i], (-45.0 * DEG2RAD) + offsetRad);
        Angles aA = posToAngle(pA);

        // Leg B (channels 20,21,22) → MR at  0°
        Point pB = rotateXY(pathMR[i], (  0.0 * DEG2RAD) + offsetRad);
        Angles aB = posToAngle(pB);

        // Leg C (channels 8,9,10)  → ML at +45°
        Point pC = rotateXY(pathML[i], (+45.0 * DEG2RAD) + offsetRad);
        Angles aC = posToAngle(pC);

        // send each leg’s 3-channel command atomically
        moveServo( 0,  1,  2, aA.J1, aA.J2, aA.J3);
        moveServo(20, 21, 22, aB.J1, aB.J2, aB.J3);
        moveServo( 8,  9, 10, aC.J1, aC.J2, aC.J3);

        // one sleep for all three legs
        std::this_thread::sleep_for(
            std::chrono::milliseconds(frameDelay)
        );
    }
}

void moveSecondTripod(double angleDeg) {
    assignCoordinatesCoxa();

    const double S = 60.0, T = 60.0;
    const int n = 10;
    const int cycleMs = 1000;
    const int frameDelay = cycleMs / (n * 3);

    auto pathML = makeFootCycle(S, T, n, -S/2.0);  // HL (dip)
    auto pathMR = makeFootCycle(S, T, n, +S/2.0);  // HR and LR (lift)

    double offsetRad = angleDeg * DEG2RAD;

    for (size_t i = 0; i < pathML.size(); ++i) {
        // HL (4,5,6) at –45° (forward, dipping)
        Point pD = rotateXY(pathML[i], (-45.0 * DEG2RAD) + offsetRad);
        Angles aD = posToAngle(pD);

        // HR (16,17,18) at –45°, reverse direction by flipping X
        Point reversedHR = pathMR[i];
        reversedHR.x = -reversedHR.x;
        Point pE  = rotateXY(reversedHR, 0.0 + offsetRad);
        Angles aE = posToAngle(pE);

        // LR (24,25,26) at +45° (forward, lifting)
        Point pF = rotateXY(pathMR[i], (+45.0 * DEG2RAD) + offsetRad);
        Angles aF = posToAngle(pF);

        moveServo( 4,  5,  6, aD.J1, aD.J2, aD.J3);
        moveServo(16, 17, 18, aE.J1, aE.J2, aE.J3);
        moveServo(24, 25, 26, aF.J1, aF.J2, aF.J3);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(frameDelay)
        );
    }
}

void rotateFirstTripodInPlaceLeft() {
    assignCoordinatesCoxa();


    // stroke parameters
    const double S = 60.0, T = 60.0;
    const int    n = 10;             // points per segment
    const int    cycleMs = 1000;     // total cycle duration (1s)
    const int    frameDelay = cycleMs / (n * 3);

    // precompute two raw foot-paths (Y=0):
    //  - back/dip for ML legs (liftZ negative)
    //  - front/lift for MR leg (liftZ positive)
    auto pathML = makeFootCycle(S, T, n, -S/2.0);
    auto pathMR = makeFootCycle(S, T, n, +S/2.0);

    for (size_t i = 0; i < pathML.size(); ++i) {
        // Leg A (channels 0,1,2)  → ML at –45°
        Point pA = rotateXY(pathML[i], DEG2RAD + M_PI);
        Angles aA = posToAngle(pA);

        // Leg B (channels 20,21,22) → MR at  0°
        Point pB = rotateXY(pathMR[i],    0.0);
        Angles aB = posToAngle(pB);

        // Leg C (channels 8,9,10)  → ML at +45°
        Point pC = rotateXY(pathML[i], DEG2RAD + M_PI);
        Angles aC = posToAngle(pC);

        // send each leg’s 3-channel command atomically
        moveServo( 0,  1,  2, aA.J1, aA.J2, aA.J3);
        moveServo(20, 21, 22, aB.J1, aB.J2, aB.J3);
        moveServo( 8,  9, 10, aC.J1, aC.J2, aC.J3);

        // one sleep for all three legs
        std::this_thread::sleep_for(
            std::chrono::milliseconds(frameDelay)
        );
    }
}

void rotateSecondTripodInPlaceLeft() {
    assignCoordinatesCoxa();

    const double S = 60.0, T = 60.0;
    const int n = 10;
    const int cycleMs = 1000;
    const int frameDelay = cycleMs / (n * 3);

    auto pathML = makeFootCycle(S, T, n, -S/2.0);  // HL (dip)
    auto pathMR = makeFootCycle(S, T, n, +S/2.0);  // HR and LR (lift)

    for (size_t i = 0; i < pathML.size(); ++i) {
        // HL (4,5,6) at –45° (forward, dipping)
        Point pD = rotateXY(pathML[i], DEG2RAD + M_PI);
        Angles aD = posToAngle(pD);

        // HR (16,17,18) at –45°, reverse direction by flipping X
        Point reversedHR = pathMR[i];
        reversedHR.x = -reversedHR.x;
        Point pE = rotateXY(reversedHR, DEG2RAD);
        Angles aE = posToAngle(pE);

        // LR (24,25,26) at +45° (forward, lifting)
        Point pF = rotateXY(pathMR[i], +45.0 * DEG2RAD);
        Angles aF = posToAngle(pF);

        moveServo( 4,  5,  6, aD.J1, aD.J2, aD.J3);
        moveServo(16, 17, 18, aE.J1, aE.J2, aE.J3);
        moveServo(24, 25, 26, aF.J1, aF.J2, aF.J3);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(frameDelay)
        );
    }
}

void rotateFirstTripodInPlaceRight() {
    assignCoordinatesCoxa();


    // stroke parameters
    const double S = 60.0, T = 60.0;
    const int    n = 10;             // points per segment
    const int    cycleMs = 1000;     // total cycle duration (1s)
    const int    frameDelay = cycleMs / (n * 3);

    // precompute two raw foot-paths (Y=0):
    //  - back/dip for ML legs (liftZ negative)
    //  - front/lift for MR leg (liftZ positive)
    auto pathML = makeFootCycle(S, T, n, -S/2.0);
    auto pathMR = makeFootCycle(S, T, n, +S/2.0);

    for (size_t i = 0; i < pathML.size(); ++i) {
        // Leg A (channels 0,1,2)  → ML at –45°
        Point pA = rotateXY(pathML[i], DEG2RAD);
        Angles aA = posToAngle(pA);

        // invert the forward/backward motion by negating X
        Point inv = pathMR[i];
        inv.x = -inv.x;                          // ← mirror the stroke :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}
        Point pB  = rotateXY(inv, 0.0);
        Angles aB = posToAngle(pB);

        // Leg C (channels 8,9,10)  → ML at +45°
        Point pC = rotateXY(pathML[i], DEG2RAD);
        Angles aC = posToAngle(pC);

        // send each leg’s 3-channel command atomically
        moveServo( 0,  1,  2, aA.J1, aA.J2, aA.J3);
        moveServo(20, 21, 22, aB.J1, aB.J2, aB.J3);
        moveServo( 8,  9, 10, aC.J1, aC.J2, aC.J3);

        // one sleep for all three legs
        std::this_thread::sleep_for(
            std::chrono::milliseconds(frameDelay)
        );
    }
}

void rotateSecondTripodInPlaceRight() {
    assignCoordinatesCoxa();

    const double S = 60.0, T = 60.0;
    const int n = 10;
    const int cycleMs = 1000;
    const int frameDelay = cycleMs / (n * 3);

    auto pathML = makeFootCycle(S, T, n, -S/2.0);  // HL (dip)
    auto pathMR = makeFootCycle(S, T, n, +S/2.0);  // HR and LR (lift)

    for (size_t i = 0; i < pathML.size(); ++i) {
        // HL (4,5,6) at –45° (forward, dipping)
        Point pD = rotateXY(pathML[i], DEG2RAD);
        Angles aD = posToAngle(pD);

        // HR (16,17,18) at –45°, reverse direction by flipping X
        //Point reversedHR = pathMR[i];
        //reversedHR.x = -reversedHR.x;
        Point pE = rotateXY(pathMR[i], DEG2RAD);
        Angles aE = posToAngle(pE);

        // LR (24,25,26) at +45° (forward, lifting)
        Point reversedHR = pathMR[i];
        reversedHR.x = -reversedHR.x;
        Point pF = rotateXY(reversedHR, DEG2RAD);
        Angles aF = posToAngle(pF);

        moveServo( 4,  5,  6, aD.J1, aD.J2, aD.J3);
        moveServo(16, 17, 18, aE.J1, aE.J2, aE.J3);
        moveServo(24, 25, 26, aF.J1, aF.J2, aF.J3);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(frameDelay)
        );
    }
}
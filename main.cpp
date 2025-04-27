#include <math.h>
#include <thread>

#include "include/Serial.h"
#include "LegCoordinates.h"
#include "include/Gait.h"
#include "include/Kinematics.h"
#include "include/Servo.h"

constexpr double DEG2RAD = M_PI/180.0;

int main() {
    assignCoordinatesCoxa();
    openSerialPort("/dev/ttyUSB0", B115200);

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

    while (true) {
        for (size_t i = 0; i < pathML.size(); ++i) {
            // Leg A (channels 0,1,2)  → ML at –45°
            Point pA = rotateXY(pathML[i], -45.0 * DEG2RAD);
            Angles aA = posToAngle(pA);

            // Leg B (channels 20,21,22) → MR at  0°
            Point pB = rotateXY(pathMR[i],    0.0);
            Angles aB = posToAngle(pB);

            // Leg C (channels 8,9,10)  → ML at +45°
            Point pC = rotateXY(pathML[i], +45.0 * DEG2RAD);
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

    closeSerialPort();
    return 0;
}
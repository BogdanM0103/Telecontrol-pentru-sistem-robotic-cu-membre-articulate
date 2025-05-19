#include <iostream>
#include <math.h>
#include <thread>

#include "include/Serial.h"
#include "LegCoordinates.h"
#include "include/Gait.h"
#include "include/Kinematics.h"
#include "include/Servo.h"

int main() {
    openSerialPort("/dev/ttyUSB0", B115200);

    while (true) {
        std::cout << "first tripod\n";
        //moveFirstTripod();
        rotateFirstTripodInPlace();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 1s pause

        std::cout << "second tripod\n";
        //moveSecondTripod();
        //rotateSecondTripodInPlace();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 1s pause

        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));  // 1s pause
    }

    closeSerialPort();
    return 0;
}
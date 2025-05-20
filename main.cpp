#include <iostream>
#include <math.h>
#include <thread>

#include "include/Serial.h"
#include "LegCoordinates.h"
#include "include/Gait.h"
#include "include/Kinematics.h"
#include "include/Servo.h"

void unstiffenIdleCoxae(const std::initializer_list<int>& channels) {
    for (int ch : channels) {
        disableServo(ch);
        // tiny delay to avoid overwhelming the serial bus
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    openSerialPort("/dev/ttyUSB0", B115200);

    //while (true) {
        std::cout << "first tripod\n";
        //moveFirstTripod();
        //unstiffenIdleCoxae({4, 16, 24});
        rotateFirstTripodInPlaceLeft();
        //rotateFirstTripodInPlaceRight();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 1s pause

        std::cout << "second tripod\n";
        //moveSecondTripod();
        //unstiffenIdleCoxae({0, 8, 20});
        rotateSecondTripodInPlaceLeft();
        //rotateSecondTripodInPlaceRight();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 1s pause

        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));  // 1s pause
    //}

    closeSerialPort();
    return 0;
}
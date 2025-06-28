// main.cpp
#include <iostream>
#include <thread>
#include <string>

#include "include/Serial.h"
#include "include/Servo.h"
#include "include/Kinematics.h"
#include "LegCoordinates.h"

void setAllServosTo90() {
    for (int ch : {0,1,2,4,5,6,8,9,10,16,17,18,20,21,22,24,25,26}) {
        // assuming 1500µs == 90°
        moveServo(ch, 90);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage:\n"
                  << "  ./HexapodRobot forward\n"
                  << "  ./HexapodRobot rotate_left\n"
                  << "  ./HexapodRobot rotate_right\n"
                  << "  ./HexapodRobot move_backward\n"
                  << "  ./HexapodRobot crab <angle_degrees>\n"
                  << "  ./HexapodRobot stop\n";
        return 1;
    }

    std::string cmd = argv[1];

    // open your serial port (updated to ttyUSB1)
    if (openSerialPort("/dev/ttyUSB0", B115200) == true || openSerialPort("/dev/ttyUSB1", B115200) == true) {
        if (cmd == "forward") {
            std::cout << ">>> Moving forward one cycle\n";
            moveFirstTripod(0.0f);
            moveSecondTripod(0.0f);
        }
        else if (cmd == "rotate_left") {
            std::cout << ">>> Rotating in place left\n";
            rotateFirstTripodInPlaceLeft();
            rotateSecondTripodInPlaceLeft();
        }
        else if (cmd == "rotate_right") {
            std::cout << ">>> Rotating in place right\n";
            rotateFirstTripodInPlaceRight();
            rotateSecondTripodInPlaceRight();
        }
        else if (cmd == "move_backward") {
            std::cout << ">>> Moving backward one cycle\n";
            moveFirstTripod(180.0f);
            moveSecondTripod(180.0f);
        }
        else if (cmd == "crab") {
            if (argc < 3) {
                std::cerr << "Usage: ./HexapodRobot crab <angle_degrees>\n";
                closeSerialPort();
                return 1;
            }
            float angle = std::stof(argv[2]);
            std::cout << ">>> Crab mode at " << angle << "°\n";
            // continuous crab walk until stopped
            while (true) {
                moveFirstTripod(angle);
                moveSecondTripod(angle);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        else if (cmd == "stop") {
            std::cout << ">>> Stopping and unpowering servos\n";
            unstiffenIdleCoxae({
                0,1,2, 4,5,6, 8,9,10,
               16,17,18,20,21,22,24,25,26
            });
            closeSerialPort();
            return 0;
        }
        else {
            std::cerr << "Unknown command: " << cmd << "\n";
            closeSerialPort();
            return 1;
        }

        // allow movement to finish one cycle
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        closeSerialPort();
    }

    return 0;
}
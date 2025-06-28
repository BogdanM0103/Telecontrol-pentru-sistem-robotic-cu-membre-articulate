// main.cpp
#include <iostream>
#include <thread>
#include <string>
#include <algorithm>

#include "include/Serial.h"
#include "include/Servo.h"
#include "include/Kinematics.h"
#include "LegCoordinates.h"

enum class Command {
    Forward,
    RotateLeft,
    RotateRight,
    MoveBackward,
    Crab,
    Stop,
    Unknown
};

Command parseCommand(const std::string& s) {
    std::string cmd = s;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    if      (cmd == "forward")         return Command::Forward;
    else if (cmd == "rotate_left")     return Command::RotateLeft;
    else if (cmd == "rotate_right")    return Command::RotateRight;
    else if (cmd == "move_backward")   return Command::MoveBackward;
    else if (cmd == "crab")            return Command::Crab;
    else if (cmd == "stop")            return Command::Stop;
    else                                return Command::Unknown;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage:\n"
                  << "  ./HexapodRobot forward\n"
                  << "  ./HexapodRobot rotate_left\n"
                  << "  ./HexapodRobot rotate_right\n"
                  << "  ./HexapodRobot move_backward\n"
                  << "  ./HexapodRobot crab <angle_degrees>\n"
                  << "  ./HexapodRobot get_up\n"
                  << "  ./HexapodRobot sit_down\n"
                  << "  ./HexapodRobot stop\n";
        return 1;
    }

    // Deschidem portul serial pe /dev/ttyUSB0 sau /dev/ttyUSB1
    if (!openSerialPort("/dev/ttyUSB0", B115200) &&
        !openSerialPort("/dev/ttyUSB1", B115200)) {
        std::cerr << "Error: could not open serial port\n";
        return 1;
    }

    Command cmd = parseCommand(argv[1]);
    float angle = 0.0f;

    if (cmd == Command::Crab) {
        if (argc < 3) {
            std::cerr << "Usage: ./HexapodRobot crab <angle_degrees>\n";
            closeSerialPort();
            return 1;
        }
        angle = std::stof(argv[2]);
    }

    switch (cmd) {
        case Command::Forward:
            std::cout << ">>> Moving forward one cycle\n";
            moveFirstTripod(0.0f);
            moveSecondTripod(0.0f);
            break;

        case Command::RotateLeft:
            std::cout << ">>> Rotating in place left\n";
            rotateFirstTripodInPlaceLeft();
            rotateSecondTripodInPlaceLeft();
            break;

        case Command::RotateRight:
            std::cout << ">>> Rotating in place right\n";
            rotateFirstTripodInPlaceRight();
            rotateSecondTripodInPlaceRight();
            break;

        case Command::MoveBackward:
            std::cout << ">>> Moving backward one cycle\n";
            moveFirstTripod(180.0f);
            moveSecondTripod(180.0f);
            break;

        case Command::Crab:
            std::cout << ">>> Crab mode at " << angle << "°\n";
            while (true) {
                moveFirstTripod(angle);
                moveSecondTripod(angle);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            // not reached
            break;

        case Command::Stop:
            std::cout << ">>> Stopping and unpowering servos\n";
            freezeAllServos();
            closeSerialPort();
            return 0;

        case Command::Unknown:
        default:
            std::cerr << "Unknown command: " << argv[1] << "\n";
            closeSerialPort();
            return 1;
    }

    // Așteptăm finalizarea mișcării și închidem portul
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    closeSerialPort();
    return 0;
}

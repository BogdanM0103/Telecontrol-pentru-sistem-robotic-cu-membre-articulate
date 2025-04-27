cmake_minimum_required(VERSION 3.25)
project(HexapodRobot)

set(CMAKE_CXX_STANDARD 20)

include_directories(HexapodRobot/include)

add_executable(HexapodRobot
        main.cpp
        Serial.cpp
        Kinematics.cpp
        Servo.cpp
        LegCoordinates.cpp
        Gait.cpp
)

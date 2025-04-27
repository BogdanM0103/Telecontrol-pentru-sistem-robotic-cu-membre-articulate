cmake_minimum_required(VERSION 3.25)
project(HexapodRobot)

set(CMAKE_CXX_STANDARD 20)

add_executable(HexapodRobot
        main.cpp
        Serial.cpp
        Types.h
        Kinematics.cpp
        Kinematics.h
        Servo.cpp
        Servo.h
        LegCoordinates.cpp
        LegCoordinates.h
        Gait.cpp
        Gait.h
)

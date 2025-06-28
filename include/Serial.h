//
// Created by Bogdan on 4/22/2025.
//
// Serial.h
#pragma once
#include <termios.h>
extern int serialPortFD;
bool openSerialPort(const char* port = "/dev/ttyUSB0", int baudRate = 112500);
void closeSerialPort();
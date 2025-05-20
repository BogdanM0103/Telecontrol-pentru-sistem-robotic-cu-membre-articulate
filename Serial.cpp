#include "include/Serial.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

int serialPortFD = -1;

void openSerialPort(const char* port, int baudRate) {
    serialPortFD = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serialPortFD == -1) {
        std::cerr << "Failed to open port: " << port << std::endl;
        return;
    }

    termios options;
    tcgetattr(serialPortFD, &options);

    cfsetispeed(&options, baudRate);
    cfsetospeed(&options, baudRate);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    tcsetattr(serialPortFD, TCSANOW, &options);

    std::cout << "Serial port opened: " << port << std::endl;
}

void closeSerialPort() {
    if (serialPortFD != -1) {
        close(serialPortFD);
        std::cout << "Serial port closed." << std::endl;
        serialPortFD = -1;
    }
}

// Send a zero-pulse to “turn off” the servo on that channel.
// (Most controllers interpret pulse==0 as “stop pulsing.”)
void disableServo(int channel) {
    if (serialPortFD == -1) {
        std::cerr << "Serial port not open.\n";
        return;
    }
    if (channel < 0 || channel > 31) {
        std::cerr << "Invalid channel number.\n";
        return;
    }
    // ASCII “#<chan>P0<CR>” => 0 µs pulse => no more pulses sent
    char cmd[16];
    snprintf(cmd, sizeof(cmd), "#%dP0\r", channel);
    write(serialPortFD, cmd, strlen(cmd));
}
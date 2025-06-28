#include "include/Serial.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdbool.h>

int serialPortFD = -1;

bool openSerialPort(const char* port, int baudRate) {
    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        std::cerr << "Failed to open port: " << port << std::endl;
        return false;
    }


    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        close(fd);
        std::cerr << "tcgetattr failed\n";
        return false;
    }

    cfsetispeed(&tty, baudRate);
    cfsetospeed(&tty, baudRate);

    // Păstrăm CLOCAL+CREAD (ignore modem lines + enable receiver)
    tty.c_cflag |= CLOCAL | CREAD;

    // Configurează portul în raw mode (8N1, fără flow‐control, fără echo etc.)
    cfmakeraw(&tty);

    // Aplicăm imediat noile atribute
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        close(fd);
        std::cerr << "tcsetattr failed\n";
        return false;
    }

    serialPortFD = fd;
    std::cout << "Serial port opened: " << port << std::endl;
    return true;
}


void closeSerialPort() {
    if (serialPortFD != -1) {
        close(serialPortFD);
        std::cout << "Serial port closed." << std::endl;
        serialPortFD = -1;
    }
}


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
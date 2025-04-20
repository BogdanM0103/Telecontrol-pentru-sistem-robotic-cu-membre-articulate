#include <algorithm>
#include <iostream>
#include <cmath>
#include <array>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define J2L 58
#define J3L 138
#define YRest 58
#define ZRest -138
#define PI 3.14159265

const double baseOffsetX =  100;  // forward from center
const double baseOffsetY =  60;   // to the right of center

using namespace std;

struct Point {
    double x;
    double y;
    double z;
}Point;

struct Angles {
    double J1;
    double J2;
    double J3;
}Angles;

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

// Transforms global (robot-centered) foot position into leg-local position
struct Point globalToLocal(struct Point globalFootPos, double baseX, double baseY) {
    struct Point local;
    local.x = globalFootPos.x - baseX;
    local.y = globalFootPos.y - baseY;
    local.z = globalFootPos.z;
    return local;
}

struct Angles posToAngle(struct Point& p) {
    double x = -p.x;
    double y = -p.y;
    double z = -p.z;

    // Apply rest offsets
    y += YRest;
    z += ZRest;

    // Correctly handle quadrant: atan2 avoids flipping
    double J1 = atan2(x, y) * (180.0 / PI);

    double H = sqrt(x * x + y * y);
    double L = sqrt(H * H + z * z);

    double cosJ3 = ((J2L * J2L) + (J3L * J3L) - (L * L)) / (2.0 * J2L * J3L);
    cosJ3 = std::clamp(cosJ3, -1.0, 1.0); // prevent domain errors
    double J3 = acos(cosJ3) * (180.0 / PI);

    double cosB = ((L * L) + (J2L * J2L) - (J3L * J3L)) / (2.0 * L * J2L);
    cosB = std::clamp(cosB, -1.0, 1.0);
    double B = acos(cosB) * (180.0 / PI);

    double A = atan2(z, H) * (180.0 / PI);
    double J2 = B + A;

    struct Angles a;
    a.J1 = 90.0 - J1;  // Turning toward X axis
    a.J2 = 90.0 + J2;  // Up/down
    a.J3 = J3 + 2.0;   // Joint compensation offset
    return a;
}

int degreesToPulseWidth(int degrees) {
    if (degrees < 0) degrees = 0;
    if (degrees > 180) degrees = 180;

    // Map 0° -> 500 µs, 180° -> 2500 µs
    return 500 + (degrees * (2000.0 / 180.0));
}

void moveServo(int channel, int degrees) {
    if (serialPortFD == -1) {
        std::cerr << "Serial port not open." << std::endl;
        return;
    }

    if (channel < 0 || channel > 31) {
        std::cerr << "Invalid channel number." << std::endl;
        return;
    }

    if (degrees < 0) degrees = 0;
    if (degrees > 180) degrees = 180;

    int pulseWidth = 500 + (int)(degrees * (2000.0 / 180.0)); // map 0–180° to 500–2500μs
    char command[32];
    snprintf(command, sizeof(command), "#%dP%dT100\r", channel, pulseWidth);

    write(serialPortFD, command, strlen(command));
}

struct Point HL;
struct Point ML;
struct Point LL;
struct Point HR;
struct Point MR;
struct Point LR;

void assignCoordinatesCoxa() {
    HL.x = -70;
    HL.y = 121.24;

    ML.x = -140;
    ML.y = 0;

    LL.x = -70;
    LL.y = -121.24;

    HR.x = 70;
    HR.y = 121.24;

    MR.x = 140;
    MR.y = 0;

    LR.x = -70;
    LR.y = 121.24;
}

int main() {

    assignCoordinatesCoxa();

    struct Point point;
    point.x = 0;
    point.y = -40;
    point.z = 0;
    struct Angles a = posToAngle(point);
    openSerialPort("/dev/ttyUSB0", 115200);
    cout << a.J1 << endl;
    cout << a.J2 << endl;
    cout << a.J3 << endl;
    moveServo(8, a.J1);
    moveServo(9, a.J2);
    moveServo(10, a.J3);
    closeSerialPort();
    return 0;
}
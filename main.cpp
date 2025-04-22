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

struct Point localToGlobal(const struct Point& localFootPos, double baseX, double baseY) {
    struct Point global;
    global.x = localFootPos.x + baseX;
    global.y = localFootPos.y + baseY;
    global.z = localFootPos.z;
    return global;
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


    //Check Coxa degrees
    if (channel == 0 || channel == 4 || channel == 8 || channel == 16 || channel == 20 || channel == 24) {
        if (degrees < 10 || degrees > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Femur degrees
    if (channel == 1 || channel == 5 || channel == 9 || channel == 117 || channel == 21 || channel == 25) {
        if (degrees < 55 || degrees > 130) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Tibia degrees
    if (channel == 1 || channel == 5 || channel == 9 || channel == 117 || channel == 21 || channel == 25) {
        if (degrees < 60 || degrees > 160) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }


    int pulseWidth = 500 + (int)(degrees * (2000.0 / 180.0)); // map 0–180° to 500–2500μs
    char command[32];
    snprintf(command, sizeof(command), "#%dP%dT100\r", channel, pulseWidth);

    write(serialPortFD, command, strlen(command));
}

void printLegAngles(const struct Point& legBase, const struct Point& globalTarget, const std::string& legName) {
    struct Point localTarget = globalToLocal(globalTarget, legBase.x, legBase.y);
    struct Angles angles = posToAngle(localTarget);

    cout << "Leg: " << legName << endl;
    cout << "  J1 (Coxa)  = " << angles.J1 << "°" << endl;
    cout << "  J2 (Femur) = " << angles.J2 << "°" << endl;
    cout << "  J3 (Tibia) = " << angles.J3 << "°" << endl;
    cout << "-----------------------------" << endl;
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

struct Point legPath(float phase, float stepLength = 30.0f, float liftHeight = 30.0f) {
    struct Point pos;
    float halfStep = stepLength / 2.0f;

    if (phase < 0.5f) {
        // === STANCE PHASE ===
        float t = phase / 0.5f; // [0, 1]
        pos.x = (1.0f - t) * halfStep + t * (-halfStep); // front to back
        pos.z = 0.0f; // on the ground
    } else {
        // === SWING PHASE ===
        float t = (phase - 0.5f) / 0.5f; // [0, 1]
        pos.x = (-halfStep) + t * stepLength; // back to front
        pos.z = liftHeight * std::sin(M_PI * t); // smooth lift and drop
    }

    pos.y = 0.0f; // No side motion for this basic path

    return pos;
}



int main() {
    assignCoordinatesCoxa();
    openSerialPort("/dev/ttyUSB0", B115200);



    closeSerialPort();
    return 0;
}
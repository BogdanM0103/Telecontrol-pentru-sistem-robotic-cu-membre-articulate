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

#include <thread>
#include <vector>

#include "Serial.h"

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
    double x = p.x;
    double y = p.y;
    double z = p.z;

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
    if (channel == 1 || channel == 5 || channel == 9 || channel == 17 || channel == 21 || channel == 25) {
        if (degrees < 55 || degrees > 130) {
            std::cerr << "Invalid degrees." << std::endl;
            return;
        }
    }

    //Check Tibia degrees
    if (channel == 2 || channel == 6 || channel == 10 || channel == 18 || channel == 22 || channel == 26) {
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

// linespace function
vector<float> linespace(float start, float end, int num_points) {
    std::vector<float> values;
    if (num_points <= 1) {
        values.push_back(start);
        return values;
    }
    float step = (end - start) / (num_points - 1);
    for (int i = 0; i < num_points; ++i) {
        values.push_back(start + step * i);
    }
    return values;
}

void animationMRLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad) {
    double S = 60;
    double A = 30;
    double T = 60;

    int numberOfPoint = 20;

    struct Point p1;
    p1.x = 0;
    p1.y = 0;
    p1.z = 0;

    struct Point p2;
    p2.x = T / 2;
    p2.y = 0;
    p2.z = S / 2;

    struct Point p3;
    p3.x = T;
    p3.y = 0;
    p3.z = 0;

    //smooth transition between P1 and P2
    std::vector<float> x_path = linespace(p1.x, p2.x, numberOfPoint);
    std::vector<float> y_path = linespace(p1.y, p2.y, numberOfPoint);
    std::vector<float> z_path = linespace(p1.z, p2.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x_path[i];
        p.y = y_path[i];
        p.z = z_path[i];

        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x2_path = linespace(p2.x, p3.x, numberOfPoint);
    std::vector<float> y2_path = linespace(p2.y, p3.y, numberOfPoint);
    std::vector<float> z2_path = linespace(p2.z, p3.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x2_path[i];
        p.y = y2_path[i];
        p.z = z2_path[i];

        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x3_path = linespace(p3.x, p1.x, numberOfPoint);
    std::vector<float> y3_path = linespace(p3.y, p1.y, numberOfPoint);
    std::vector<float> z3_path = linespace(p3.z, p1.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x3_path[i];
        p.y = y3_path[i];
        p.z = z3_path[i];

        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }
}

void animationMLLeg(int coxaPin, int femurPin, int tibiaPin, double angleRad) {
    double S = 60;
    double A = 30;
    double T = 60;

    int numberOfPoint = 40;

    struct Point p1;
    p1.x = 0;
    p1.y = 0;
    p1.z = 0;

    struct Point p2;
    p2.x = T / 2;
    p2.y = 0;
    p2.z = -S / 2;

    struct Point p3;
    p3.x = T;
    p3.y = 0;
    p3.z = 0;

    //smooth transition between P1 and P2
    std::vector<float> x_path = linespace(p1.x, p2.x, numberOfPoint);
    std::vector<float> y_path = linespace(p1.y, p2.y, numberOfPoint);
    std::vector<float> z_path = linespace(p1.z, p2.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x_path[i];
        p.y = y_path[i];
        p.z = z_path[i];

        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x2_path = linespace(p2.x, p3.x, numberOfPoint);
    std::vector<float> y2_path = linespace(p2.y, p3.y, numberOfPoint);
    std::vector<float> z2_path = linespace(p2.z, p3.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x2_path[i];
        p.y = y2_path[i];
        p.z = z2_path[i];

        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }

    //smooth transition between P1 and P2
    std::vector<float> x3_path = linespace(p3.x, p1.x, numberOfPoint);
    std::vector<float> y3_path = linespace(p3.y, p1.y, numberOfPoint);
    std::vector<float> z3_path = linespace(p3.z, p1.z, numberOfPoint);
    for (int i = 0; i < numberOfPoint; ++i) {
        struct Point p;
        p.x = x3_path[i];
        p.y = y3_path[i];
        p.z = z3_path[i];

        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;

        struct Angles a = posToAngle(p);
        moveServo(coxaPin, a.J1);
        moveServo(femurPin, a.J2);
        moveServo(tibiaPin, a.J3);

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // smooth transition
    }
}

void test(int coxaPin, int femurPin, int tibiaPin, double rotationAngle) {
    struct Point p;
    p.x = 0;
    p.y = 0;
    p.z = 20;

    struct Angles a = posToAngle(p);
    moveServo(coxaPin, a.J1);
    moveServo(femurPin, a.J2);
    moveServo(tibiaPin, a.J3);
}

struct Point rotateXY(const struct Point& p, double angleRad) {
    struct Point result;
    result.x = p.x * cos(angleRad) - p.y * sin(angleRad);
    result.y = p.x * sin(angleRad) + p.y * cos(angleRad);
    result.z = p.z;
    return result;
}

int main() {
    assignCoordinatesCoxa();
    openSerialPort("/dev/ttyUSB0", B115200);

    while (true) {
        animationMLLeg(4, 5, 6, 30);
    }

    //animationMRLeg(20, 21, 22, 30);
    closeSerialPort();

    return 0;
}
#include "include/Serial.h"
#include "LegCoordinates.h"
#include "include/Gait.h"

int main() {
    assignCoordinatesCoxa();
    openSerialPort("/dev/ttyUSB0", B115200);

    while (true) {
        animationMRLeg(20, 21, 22, 30.0);
        // animationMLLeg(4, 5, 6, 30.0);
    }

    closeSerialPort();
    return 0;
}
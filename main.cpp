#include "Serial.h"
#include "LegCoordinates.h"
#include "Gait.h"

int main() {
    assignCoordinatesCoxa();
    openSerialPort("/dev/ttyUSB0", B115200);

    while (true) {
        animationMLLeg(4, 5, 6, 30.0);
    }

    closeSerialPort();
    return 0;
}
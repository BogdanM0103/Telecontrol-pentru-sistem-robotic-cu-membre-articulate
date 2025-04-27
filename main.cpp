#include <math.h>
#include <thread>

#include "include/Serial.h"
#include "LegCoordinates.h"
#include "include/Gait.h"

constexpr double DEG2RAD = M_PI/180.0;

int main() {
    assignCoordinatesCoxa();
    openSerialPort("/dev/ttyUSB0", B115200);


        //animationMRLeg(20, 21, 22, 30.0);
        std::thread t1(animationMLLeg, 0,1,2,  -45 * DEG2RAD);
        std::thread t2(animationMRLeg, 20,21,22, 0 * DEG2RAD);
        std::thread t3(animationMLLeg, 8,9, 10, 45 * DEG2RAD);
        t1.join();
        t2.join();
        t3.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    closeSerialPort();
    return 0;
}
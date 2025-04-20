#include <iostream>
#include <cmath>
#include <array>

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
};

void posToAngle(double x, double y, double z) {
    y += YRest;
    z += ZRest;

    // CALCULATE INVERSE KINEMATIC SOLUTION
    double J1 = atan(x / y) * (180 / PI);
    double H = sqrt((y * y) + (x * x));
    double L = sqrt((H * H) + (z * z));
    double J3 = acos(   ((J2L * J2L) + (J3L * J3L) - (L * L))   /   (2 * J2L * J3L)   ) * (180 / PI);
    double B = acos(   ((L * L) + (J2L * J2L) - (J3L * J3L))   /   (2 * L * J2L)   ) * (180 / PI);
    double A = atan(z / H) * (180 / PI);  // BECAUSE Z REST IS NEGATIVE, THIS RETURNS A NEGATIVE VALUE
    double J2 = (B + A);  // BECAUSE 'A' IS NEGATIVE AT REST WE NEED TO INVERT '-' TO '+'

    cout << 90 - J1 << endl;
    cout << 90 + J2 << endl;
    cout << J3 + 2 << endl;

}

int main() {
    struct Point point;
    point.x = 0;
    point.y = 0;
    point.z = 0;

    posToAngle(point.x, point.y, point.z);
    return 0;
}
#include "adxl345.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    try {
        ADXL345 accel;

        while (true) {
            float x, y, z;
            accel.readXYZ(x, y, z);
            std::cout << "X=" << x
                      << " g  Y=" << y
                      << " g  Z=" << z << " g\n";
            usleep(100000);
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

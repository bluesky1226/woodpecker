#include "adxl345.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unistd.h>

int main() {
    try {
        ADXL345 accel;

        std::ostringstream filename;
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        filename << "adxl345_"
                 << std::put_time(&tm, "%Y%m%d_%H%M%S")
                 << ".csv";

        std::ofstream file(filename.str());
        if (!file.is_open()) {
            std::cerr << "Failed to open file\n";
            return 1;
        }

        file << "timestamp_us,x_g,y_g,z_g\n";

        while (true) {
            float x, y, z;
            accel.readXYZ(x, y, z);

            auto now = std::chrono::steady_clock::now();
            auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
                          now.time_since_epoch()).count();

            file << ts << "," << x << "," << y << "," << z << "\n";
            usleep(100000);
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

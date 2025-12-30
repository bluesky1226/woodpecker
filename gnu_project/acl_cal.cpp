
#include "adxl345.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <chrono>
#include <map>

using namespace std;

int main() {
    string device = "/dev/spidev1.2";
    map<string, int> cs_dic = {
        {"/dev/spidev0.0", 5},
        {"/dev/spidev0.0", 6},
        {"/dev/spidev1.0", 26}
    };
    int rtc = -1;

    try {
        ADXL345 accel(device, -1);
        rtc = accel.check_linux_device();
        if (rtc < 0) {
            printf("ADXL345 linux device failed!!\n");
            return -1;
        }

        rtc = accel.check_adxl345_link();
        if (rtc < 0) {
            printf("ADXL345 link failed!!\n");
            return -1;
        }

        if (rtc >= 0) {
            printf("ADXL345 init successfully\n");
        }

        accel.initialization();

        auto regs = accel.readAllRegisters();
        for (size_t i = 0; i < regs.size(); i++) {
            printf("0x%02zX : 0x%02X\n", i, regs[i]);
        }

        std::cout << "Starting to do the calibation ...\n";
        sleep(1);
        accel.offsetreg_value();
        float x, y, z;
        float x_t, y_t, z_t;
        x_t = 0;
        y_t = 0;
        z_t = 0;
        sleep(1);
        for (int i = 0; i < REPEAT; i++)
        {
            accel.readXYZ(x, y, z);
            // std::cout << x << "," << y << "," << z << "\n";
            x_t += x;
            y_t += y;
            z_t += z;
            printf("X:%.4f, Y:%.4f, Z:%.4f, X_TOL:%.4f, Y_TOL:%.4f, Z_TOL:%.4f\n", x, y, z, x_t, y_t, z_t);
            // std::cout << x_t << "," << y_t << "," << z_t << "\n";
            sleep(1);
        }
        float x_avg, y_avg, z_avg;
        x_avg = x_t / REPEAT;
        y_avg = y_t / REPEAT;
        z_avg = z_t / REPEAT;
        printf("X axis average : %.4f\n", x_avg);
        printf("Y axis average : %.4f\n", y_avg);
        printf("Z axis average : %.4f\n", z_avg);
        accel.adxl345_write_offsets(x_avg, y_avg, z_avg);
        printf("ADXL345 Calibration is done\n");
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

#include "adxl345.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <chrono>

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

        ostringstream filename;
        auto t = time(nullptr);
        auto tm = *localtime(&t);

        filename << "adxl345_"
                 << put_time(&tm, "%Y%m%d_%H%M%S")
                 << ".csv";

        ofstream file(filename.str());
        // ofstream file("adxl345.csv");
        if (!file.is_open()) {
            cerr << "Failed to open CSV file\n";
            return 1;
        }

        // CSV header
        file << "x_g,y_g,z_g\n";

        // ===== Start time =====
        auto start = chrono::steady_clock::now();
        constexpr int DURATION_SEC = 60;
        // ======================

        while (true) {
            float x, y, z;
            accel.readXYZ(x, y, z);

            auto now = chrono::steady_clock::now();
            auto ts = chrono::duration_cast<chrono::microseconds>(
                          now.time_since_epoch())
                          .count();

            file << x << "," << y << "," << z << "\n";
            file.flush();

            // ===== Stop after 20 seconds =====
            auto elapsed = chrono::duration_cast<chrono::seconds>(
                               now - start)
                               .count();
            if (elapsed >= DURATION_SEC) {
                break;
            }
            // =================================

            printf("elapsed: %d\n", elapsed);
            usleep(1000000); // 1000 Hz
        }

        printf("Data collection finished (%d seconds).\n", DURATION_SEC);
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}

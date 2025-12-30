
#include "adxl345.h"
#include "etgnuplot.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <chrono>
#include <vector>
#include <cstdio>
#include <csignal>

using namespace std;

int main()
{
    ETGNUPLOT gnp;
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

        while (true)
        {
            accel.collect_xyz_data();
            gnp.plot_data("CHL_X", accel.vx, 1);
            gnp.plot_data("CHL_Y", accel.vx, 2);
            gnp.plot_data("CHL_Z", accel.vx, 3);
            gnp.plot_data_2d("CHL_XY", accel.vx, accel.vy, 1);
            gnp.plot_data_2d("CHL_XZ", accel.vx, accel.vy, 2);
            gnp.plot_data_2d("CHL_YZ", accel.vx, accel.vy, 3);
            gnp.plot_data_3d("CHL_3D", accel.vx, accel.vy, accel.vz);

            usleep(1000); // 1000 Hz
        }
        gnp.cleanup(1);
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}

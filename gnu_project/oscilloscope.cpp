
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
    vector<ADXL345*> adxl345_list = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr
    };

    vector<string> lnxdev_list = {
        "/dev/spidev0.0",
        "/dev/spidev0.1",
        "/dev/spidev1.0",
        "/dev/spidev1.1",
        "/dev/spidev1.2"
    };

    map<string, ETGNUPLOT*> etgnuplot_dic = {
        {"1", nullptr},
        {"2", nullptr},
        {"3", nullptr},
        {"4", nullptr},
        {"5", nullptr}
    };

    map<string, int> cs_dic = {
        {"/dev/spidev0.0", 5},
        {"/dev/spidev0.0", 6},
        {"/dev/spidev1.0", 26}
    };

    try {
        for (int i = 0; i < etgnuplot_dic.size(); i++) {
            etgnuplot_dic[to_string(i)] = new ETGNUPLOT();
        }
        int ct = 0;
        for (const string &item : lnxdev_list) {
            ADXL345 *tmp = new ADXL345(item, -1);
            int rtc = -1;

            rtc = tmp->check_linux_device();
            if (rtc < 0) {
                adxl345_list[ct] = nullptr;
                printf("%s: ADXL345 linux device failed!!\n", item.c_str());
                ct++;
                continue;
            }

            rtc = tmp->check_adxl345_link();
            if (rtc < 0) {
                adxl345_list[ct] = nullptr;
                printf("%s: ADXL345 link failed!!\n", item.c_str());
                ct++;
                continue;
            }

            if (rtc >= 0) {
                printf("%s: ADXL345 init successfully\n", item.c_str());
                tmp->initialization();
                auto regs = tmp->readAllRegisters();
                for (size_t i = 0; i < regs.size(); i++) {
                    printf("0x%02zX : 0x%02X\n", i, regs[i]);
                }
                adxl345_list[ct] = tmp;
            }
            ct++;
            sleep(2);
        }

        while (true)
        {
            for (int i = 0; i < adxl345_list.size(); i++) {
                if (adxl345_list[i] != nullptr) {
                    adxl345_list[i]->collect_xyz_data();
                    if (etgnuplot_dic[to_string(i)] != nullptr) {
                        etgnuplot_dic[to_string(i)]->plot_data_3d("CHL_3D", adxl345_list[i]->vx, adxl345_list[i]->vy, adxl345_list[i]->vz);
                    }
                }
            }

            usleep(1000); // 1000 Hz
        }
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}

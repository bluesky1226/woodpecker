#include "nebula.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <unistd.h>
#include <cstdint>

#define SAMPLE_RATE    1000 // usleep(1000), 1kHz
#define ADXL345_NULL   -2000
#define GNUPLOT_NULL   -3000
#define BIT0           1
#define BIT1           2
#define BIT2           4
#define BIT3           8
#define BIT4           16
#define BIT5           32
#define BIT6           64
#define BIT7           128

using namespace std;

NebulaShell::NebulaShell()
{
    adxl345_dic["1"] = new ADXL345("/dev/spidev0.0", 5000000);
    // adxl345_dic["2"] = new ADXL345("/dev/spidev1.0", 5000000);
    // adxl345_dic["3"] = new ADXL345("/dev/spidev2.0", 5000000);

    etgnuplot_dic["1"] = new ETGNUPLOT();
}

NebulaShell::~NebulaShell()
{
}

// The logic that decides: System Command vs. User Command
void NebulaShell::extract_command_name(stringstream& args)
{
    string token;
    while (args >> token) {
        if (token == "savedata") {
            exec_savedata(args);
        } else if (token == "plotaxis") {
            exec_plotaxis(args);
        } else if (token == "plotclose") {
            printf("plotclose\n");
        }
    }
}

int NebulaShell::exec_savedata(stringstream& args)
{
    string token;
    string num_dev = "1";
    ADXL345 *padxl = nullptr;
    int sv_period = 10;
    float x, y, z;

    if (adxl345_dic["1"] != nullptr) {
        padxl = adxl345_dic["1"];
    }

    while (args >> token) {
        if (token == "-dev") {
            args >> token;
            if (adxl345_dic[token] != nullptr) {
                padxl = adxl345_dic[token];
            } else {
                return ADXL345_NULL;
            }
            num_dev = token;
        } else if (token == "-t") {
            args >> token;
            sv_period = stoi(token);
        }
    }

    try {
        ostringstream filename;
        auto t = time(nullptr);
        auto tm = *localtime(&t);

        filename << "adxl345_"
                 << num_dev
                 << "_"
                 << put_time(&tm, "%Y%m%d_%H%M%S")
                 << ".csv";

        ofstream file(filename.str());
        if (!file.is_open()) {
            cerr << "Failed to open CSV file\n";
            return 1;
        }

        // ===== Start time =====
        auto start = chrono::steady_clock::now();
        // ======================

        while (true) {
            padxl->readXYZ(x, y, z);

            auto now = chrono::steady_clock::now();
            auto ts = chrono::duration_cast<chrono::microseconds>(
                          now.time_since_epoch())
                          .count();

            file << x << "," << y << "," << z << "\n";
            file.flush();

            auto elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
            if (elapsed >= sv_period) {
                break;
            }

            usleep(SAMPLE_RATE);
        }

        printf("Data collection finished (%d seconds).\n", sv_period);
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}

int NebulaShell::exec_plotaxis(stringstream& args)
{
    string token;
    string num_dev = "1";
    ADXL345 *padxl = nullptr;
    ETGNUPLOT *pgnp = nullptr;
    string chart;

    if (adxl345_dic["1"] != nullptr) {
        padxl = adxl345_dic["1"];
    }

    if (etgnuplot_dic["1"] != nullptr) {
        pgnp = etgnuplot_dic["1"];
    }

    while (args >> token) {
        if (token == "-dev") {
            args >> token;
            if (adxl345_dic[token] != nullptr) {
                padxl = adxl345_dic[token];
            } else {
                return ADXL345_NULL;
            }
            if (etgnuplot_dic[token] != nullptr) {
                pgnp = etgnuplot_dic[token];
            } else {
                return GNUPLOT_NULL;
            }
            num_dev = token;
        } else if (token == "-axis") {
            args >> token;
            chart = token;
        }
    }

    printf("%s\n", chart);
    uint8_t chart_byte = static_cast<uint8_t>(stoul(chart, nullptr, 16));
    try
    {
        while (true)
        {
            padxl->collect_xyz_data();
            if (chart_byte & BIT0) {
                pgnp->plot_data("CHL_X", padxl->vx, 1);
            }

            if (chart_byte & BIT1) {
                pgnp->plot_data("CHL_Y", padxl->vx, 2);
            }

            if (chart_byte & BIT2) {
                pgnp->plot_data("CHL_Z", padxl->vx, 3);
            }

            if (chart_byte & BIT3) {
                pgnp->plot_data_2d("CHL_XY", padxl->vx, padxl->vy, 1);
            }

            if (chart_byte & BIT4) {
               pgnp->plot_data_2d("CHL_XZ", padxl->vx, padxl->vy, 2);
            }

            if (chart_byte & BIT5) {
                pgnp->plot_data_2d("CHL_YZ", padxl->vx, padxl->vy, 3);
            }

            if (chart_byte & BIT6) {
                pgnp->plot_data_3d("CHL_3D", padxl->vx, padxl->vy, padxl->vz);
            }

            usleep(SAMPLE_RATE); // 1000 Hz
        }
        pgnp->cleanup(1);
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 1;
}


#include "adxl345.h"
#include "etgnuplot.hpp"
#include <vector>
#include <cstdio>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unistd.h>
#include <iostream>

using namespace std;

ETGNUPLOT::ETGNUPLOT()
{
    initialization();
}

ETGNUPLOT::~ETGNUPLOT()
{
    cleanup(1);
}

int ETGNUPLOT::initialization()
{
    for (const string &item : plot_axis)
    {
        // gp_dic[item] = popen("gnuplot -persist", "w");
        gp_dic[item] = popen("gnuplot", "w");
        if (gp_dic[item] == nullptr)
        {
            return 1;
        }
    }

    // Initial GNUPlot setup commands
    vector<string> plotcmds_common = {
        "set title 'ADXL345 Live SPI Data'\n",
        "set xlabel 'Time Step'\n",
        "set ylabel 'Raw Acceleration'\n",
        "set grid\n"
    };

    plotcmds_common.push_back("set xrange [0:" + std::to_string(MAX_POINTS) + "]\n");
    for (const string& cmd : plotcmds_common)
    {
        fprintf(gp_dic["CHL_X"], "%s", cmd.c_str());
        fprintf(gp_dic["CHL_Y"], "%s", cmd.c_str());
        fprintf(gp_dic["CHL_Z"], "%s", cmd.c_str());
    }

    vector<string> plotcmds_2d = {
        "set title '2D Orientation'\n"
    };

    for (const string& cmd : plotcmds_2d) 
    {
        fprintf(gp_dic["CHL_XY"], "%s", cmd.c_str());
        fprintf(gp_dic["CHL_XZ"], "%s", cmd.c_str());
        fprintf(gp_dic["CHL_YZ"], "%s", cmd.c_str());
    }

    vector<string> plotcmds_3d = {
        "set title '3D Orientation'\n"
    };

    for (const string& cmd : plotcmds_3d) 
    {
        fprintf(gp_dic["CHL_3D"], "%s", cmd.c_str());
    }

    for (const string &item : plot_axis)
    {
        fflush(gp_dic[item]);
    }

    return 0;
}

void ETGNUPLOT::set_plot_limit(FILE* fp, const vector<float> vdata, int type)
{
    float v_min, v_max, v_avg, v_sum;
    int v_min_border, v_max_border;
    string stp;

    auto result = minmax_element(vdata.begin(), vdata.end());
    v_min = *result.first;
    v_max = *result.second;
    v_sum = accumulate(vdata.begin(), vdata.end(), 0.0);
    v_avg = v_sum / vdata.size();

    if (abs(v_avg - v_max) < 1)
    {
        v_max = v_avg + 1.5;
    }
    else
    {
        v_max = v_avg + abs(v_avg - v_max)*1.5;
    }

    if (abs(v_avg - v_min) < 1)
    {
        v_min = v_avg - 1.5;
    }
    else
    {
        v_min = v_avg - abs(v_avg - v_min)*1.5;
    }

    if (v_min >= 0)
    {
        v_min_border = ceil(v_min);
    }
    else
    {
        v_min_border = floor(v_min);
    }

    if (v_max >= 0)
    {
        v_max_border = ceil(v_max);
    }
    else
    {
        v_max_border = floor(v_max);
    }

    if (type == 1)
    {
        stp = "xrange";
    }
    else if (type == 2)
    {
        stp = "yrange";
    }
    else if (type == 3)
    {
        stp = "zrange";
    }
    else
    {
        stp = "";
    }

    fprintf(fp, "set %s [%d:%d]\n", stp.c_str(), v_min_border, v_max_border);
    // printf("set %s [%d:%d]\n", stp.c_str(), v_min_border, v_max_border);
}

void ETGNUPLOT::plot_data(string channel, const vector<float> vdata, int type)
{
    FILE *fp;
    fp = gp_dic[channel];

    if (type == 1)
    {
        set_plot_limit(fp, vdata, 2);
        fprintf(fp, "plot '-' with lines title 'X' lc rgb '#39FF14'\n");
    }
    else if (type == 2)
    {
        set_plot_limit(fp, vdata, 2);
        fprintf(fp, "plot '-' with lines title 'Y' lc rgb '#00FFFF'\n");
    }
    else if (type == 3)
    {
        set_plot_limit(fp, vdata, 2);
        fprintf(fp, "plot '-' with lines title 'Z' lc rgb '#FF00FF'\n");
    }

    for (int i = 0; i < vdata.size(); ++i)
    {
        fprintf(fp, "%d %.4f\n", i, vdata[i]);
    }
    fprintf(fp, "e\n");
    fflush(fp);
}

void ETGNUPLOT::plot_data_2d(string channel, const vector<float> xdata, const vector<float> ydata, int type)
{
    FILE *fp;
    fp = gp_dic[channel];

    if (type == 1)
    {
        // XY plan
        set_plot_limit(fp, xdata, 1);
        set_plot_limit(fp, ydata, 2);
        fprintf(fp, "plot '-' with points pt 7 ps 1 lc rgb 'blue' title 'XY Distribution'\n");
    }
    else if (type == 2)
    {
        // XZ plan
        set_plot_limit(fp, xdata, 1);
        set_plot_limit(fp, ydata, 3);
        fprintf(fp, "plot '-' with points pt 7 ps 1 lc rgb 'blue' title 'XZ Distribution'\n");
    }
    else if (type == 3)
    {
        // YZ plan
        set_plot_limit(fp, xdata, 2);
        set_plot_limit(fp, ydata, 3);
        fprintf(fp, "plot '-' with points pt 7 ps 1 lc rgb 'blue' title 'YZ Distribution'\n");
    }

    for (int i = 0; i < xdata.size(); ++i)
    {
        fprintf(fp, "%.4f %.4f\n", xdata[i], ydata[i]);
    }
    fprintf(fp, "e\n");
    fflush(fp);
}

void ETGNUPLOT::plot_data_3d(string channel, const vector<float> xdata, const vector<float> ydata, const vector<float> zdata)
{
    FILE *fp;
    fp = gp_dic[channel];

    set_plot_limit(fp, xdata, 1);
    set_plot_limit(fp, ydata, 2);
    set_plot_limit(fp, zdata, 3);
    fprintf(fp, "splot '-' with points pt 7 ps 1\n");

    for (int i = 0; i < xdata.size(); ++i)
    {
        fprintf(fp, "%.4f %.4f %.4f\n", xdata[i], ydata[i], zdata[i]);
    }
    fprintf(fp, "e\n");
    fflush(fp);
}

void ETGNUPLOT::subplot_xyz(string channel, const vector<float> xdata, const vector<float> ydata, const vector<float> zdata)
{
    FILE *fp;
    fp = gp_dic[channel];

    fprintf(fp, "set multiplot layout 3,1\n");
    // Subplot 1: X
    fprintf(fp, "set tmargin 2\n"); // Add some space at top
    fprintf(fp, "plot '-' with lines lc rgb '#39FF14' title 'X'\n");
    for(int i = 0; i < xdata.size(); ++i)
    {
        fprintf(fp, "%d %.4f\n", i, xdata[i]);
    }
    fprintf(fp, "e\n");

    // Subplot 2: Y
    fprintf(fp, "plot '-' with lines lc rgb '#00FFFF' title 'Y'\n");
    for(int i = 0; i < ydata.size(); ++i)
    {
        fprintf(fp, "%d %.4f\n", i, ydata[i]);
    }
    fprintf(fp, "e\n");

    // Subplot 3: Z
    fprintf(fp, "plot '-' with lines lc rgb '#FF00FF' title 'Z'\n");
    for(int i = 0; i < zdata.size(); ++i)
    {
        fprintf(fp, "%d %.4f\n", i, zdata[i]);
    }
    fprintf(fp, "e\n");
    fflush(fp);
}

void ETGNUPLOT::cleanup(int signum)
{
    std::cout << "\nInterrupt signal (" << signum << ") received. Closing Gnuplot...\n";

    for (const string &item : plot_axis)
    {
        if (gp_dic[item] != nullptr)
        {
            fprintf(gp_dic[item], "q\n");
            sleep(1);
            fflush(gp_dic[item]);
            sleep(1);
            if (gp_dic[item] == nullptr)
            {
                printf("%s is NULL\n", item);
            }
            else
            {
                printf("%s is not NULL\n", item);
                pclose(gp_dic[item]);
            }
        }
    }
}

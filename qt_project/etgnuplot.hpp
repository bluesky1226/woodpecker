#ifndef ETGNUPLOT_HPP
#define ETGNUPLOT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <map>

using namespace std;

class ETGNUPLOT {
public:
    ETGNUPLOT();
    ~ETGNUPLOT();

    int initialization();
    void set_plot_limit(FILE* fp, const vector<float> vdata, int type);
    void plot_data(string channel, const vector<float> vdata, int type);
    void plot_data_2d(string channel, const vector<float> xdata, const vector<float> ydata, int type);
    void plot_data_3d(string channel, const vector<float> xdata, const vector<float> ydata, const vector<float> zdata);
    void subplot_xyz(string channel, const vector<float> xdata, const vector<float> ydata, const vector<float> zdata);
    void cleanup(int signum);

private:
    // vfp[0] = X  // vfp[1] = Y  // vfp[2] = Z
    // vfp[3] = XY // vfp[4] = XZ // vfp[5] = YZ
    // vfp[6] = XYZ

    vector<string> plot_axis = {"CHL_X", "CHL_Y", "CHL_Z", "CHL_XY", "CHL_XZ", "CHL_YZ", "CHL_3D"};
    map<string, FILE*> gp_dic = {
        {"CHL_X", nullptr},
        {"CHL_Y", nullptr},
        {"CHL_Z", nullptr},
        {"CHL_XY", nullptr},
        {"CHL_XZ", nullptr},
        {"CHL_YZ", nullptr},
        {"CHL_3D", nullptr}
    };
};

#endif

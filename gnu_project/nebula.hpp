
#include "adxl345.hpp"
#include "etgnuplot.hpp"
#include <vector>
#include <string>
#include <map>

using namespace std;

class NebulaShell {
public:
    vector<float> vx; // Your data vector
    map<string, ADXL345*> adxl345_dic = {
        {"1", nullptr},
        {"2", nullptr},
        {"3", nullptr}
    };

    map<string, ETGNUPLOT*> etgnuplot_dic = {
        {"1", nullptr},
        {"2", nullptr},
        {"3", nullptr}
    };
 
    NebulaShell();
    ~NebulaShell();

    void extract_command_name(stringstream& args);
    int exec_savedata(stringstream& args);
    int exec_plotaxis(stringstream& args);
private:

};


#include "nebula.hpp"
#include <iostream>
#include <sstream>

using namespace std;

int main()
{
    NebulaShell nsh;
    string line;

    while (true) {
        cout << "PlotShell> ";
        getline(cin, line);

        if (line == "exit") {
            break;
        }

        stringstream ss(line);
        nsh.extract_command_name(ss);
    }

    return 0;
}


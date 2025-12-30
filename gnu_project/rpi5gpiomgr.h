
#include <gpiod.hpp>
#include <memory>

using namespace std;

class RPI5GPIOMGR {
public:
    RPI5GPIOMGR(int ics_gpio);
    ~RPI5GPIOMGR();

    void set_cs(bool action);

private:
    int cs_gpio;
    unique_ptr<gpiod::line_request> cs_request;
};
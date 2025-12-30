
#include "rpi5gpiomgr.h"
#include <unistd.h>

using namespace std;

RPI5GPIOMGR::RPI5GPIOMGR(int ics_gpio)
{
    cs_gpio = ics_gpio;
    // 1. Open the chip
    auto chip = gpiod::chip("/dev/gpiochip4");
    printf("initialization gpiochip4\n");

    // 2. Configure the line settings (e.g., Output)
    auto settings = gpiod::line_settings()
        .set_direction(gpiod::line::direction::OUTPUT);

    printf("initialization settings\n");
    // 3. Map settings to specific offsets
    auto config = gpiod::line_config();
    config.add_line_settings(cs_gpio, settings);

    printf("initialization config\n");
    // 4. SUBMIT the request (This is how you get a line_request object)
    auto request = chip.prepare_request()
        .set_consumer("my_toggle_app")
        .set_line_config(config).do_request(); // This returns the gpiod::line_request object

    printf("initialization request\n");
    cs_request = make_unique<gpiod::line_request>(move(request));
    cs_request->set_value(cs_gpio, gpiod::line::value::ACTIVE);
    printf("set GPIO to HIGH\n");
}

RPI5GPIOMGR::~RPI5GPIOMGR()
{
}

void RPI5GPIOMGR::set_cs(bool action)
{
    if (action == true) {
        // set Voltage HIGH
        cs_request->set_value(cs_gpio, gpiod::line::value::ACTIVE);
    } else {
        // set Voltage LOW
        cs_request->set_value(cs_gpio, gpiod::line::value::INACTIVE);
    }
}

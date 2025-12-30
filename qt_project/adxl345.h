#ifndef ADXL345_HPP
#define ADXL345_HPP

#include "rpi5gpiomgr.h"
#include <cstdint>
#include <string>
#include <vector>
#include <linux/spi/spidev.h>
#include <gpiod.hpp>

#define NUM_CHART             7
#define REPEAT                20
#define MAX_POINTS            1000
#define ADXL345_DEVID         0x00
#define ADXL345_OFSX          0x1E
#define ADXL345_OFSY          0x1F
#define ADXL345_OFSZ          0x20
#define ADXL345_BW_RATE       0x2C
#define ADXL345_POWER_CTL     0x2D
#define ADXL345_DATA_FORMAT   0x31
#define OFFSET_G_PER_LSB      0.0156   // 15.6 mg

using namespace std;

class ADXL345 {
public:
    vector<float> vx, vy, vz;
    RPI5GPIOMGR *gpio_mgr = nullptr;

    ADXL345(string dev, int ics_gpio);
    ~ADXL345();

    int check_linux_device();
    int check_adxl345_link();
    int initialization();
    void readXYZ(float& x, float& y, float& z);
    std::vector<uint8_t> readAllRegisters();
    int8_t calc_offset_reg(float measured_g);
    void adxl345_write_offsets(float x_g, float y_g, float z_g);
    void offsetreg_value();
    void collect_xyz_data();
    void set_cs(bool action);

private:
    int spi_fd;
    string device;
    int cs_gpio;

    uint8_t mode = SPI_MODE_3;
    uint8_t bits = 8;
    uint32_t speed_hz = 5000000; // 5 MHz

    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    int16_t readWord(uint8_t reg);
};

#endif

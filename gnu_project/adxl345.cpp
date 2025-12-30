#include "adxl345.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <cmath>
#include <gpiod.hpp>

using namespace std;

ADXL345::ADXL345(string dev, int ics_gpio)
{
    device = dev;
    cs_gpio = ics_gpio;
    if (ics_gpio > -1) {
        gpio_mgr = new RPI5GPIOMGR(ics_gpio);
    }

    printf("ADXL345 device: %s\n", device.c_str());
}

ADXL345::~ADXL345() {
    close(spi_fd);
}

int ADXL345::check_linux_device()
{
    spi_fd = open(device.c_str(), O_RDWR);
    if (spi_fd < 0) {
        printf("%s: Failed to open SPI device\n", device.c_str());
        return -1;
    }

    return 0;
}

int ADXL345::check_adxl345_link()
{
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz);

    // Verify device ID
    if (readRegister(ADXL345_DEVID) != 0xE5) {
        printf("%s: ADXL345 not detected\n", device.c_str());
        return -1;
    }

    return 0;
}

void ADXL345::set_cs(bool action)
{
    if (gpio_mgr == nullptr) {
        return;
    }

    if (action == true) {
        // Set Voltage HIGH
        gpio_mgr->set_cs(action);
        usleep(1000);
    } else {
        // Set Voltage LOW
        usleep(1000);
        gpio_mgr->set_cs(action);
    }
}

int ADXL345::initialization()
{
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz);

    // Measurement mode
    writeRegister(ADXL345_POWER_CTL, 0x08);
    sleep(1);

    // Set ODR = 3200 Hz
    writeRegister(ADXL345_BW_RATE, 0x0F);
    sleep(1);

    // Set g-Range = +/- 2g, Full resolution
    // writeRegister(ADXL345_DATA_FORMAT, 0x08);
    writeRegister(ADXL345_DATA_FORMAT, 0x0B);
    sleep(1);

    return 0;
}

vector<uint8_t> ADXL345::readAllRegisters() {
    constexpr uint8_t startReg = 0x00;
    constexpr size_t regCount = 0x3A; // 0x00–0x39

    vector<uint8_t> tx(regCount + 1, 0x00);
    vector<uint8_t> rx(regCount + 1, 0x00);

    tx[0] = 0x80 | 0x40 | startReg; // READ + MULTI + start

    struct spi_ioc_transfer tr{};
    tr.tx_buf = (unsigned long)tx.data();
    tr.rx_buf = (unsigned long)rx.data();
    tr.len = tx.size();
    tr.speed_hz = speed_hz;
    tr.bits_per_word = 8;

    set_cs(false);
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        throw runtime_error("SPI transfer failed");
    }
    set_cs(true);

    // rx[0] is junk (command echo), real data starts at rx[1]
    return vector<uint8_t>(rx.begin() + 1, rx.end());
}

void ADXL345::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t tx[2] = { static_cast<uint8_t>(reg & 0x3F), value };

    struct spi_ioc_transfer tr{};
    tr.tx_buf = (unsigned long)tx;
    tr.len = 2;
    tr.speed_hz = speed_hz;
    tr.bits_per_word = 8;

    set_cs(false);
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        throw runtime_error("SPI transfer failed");
    }
    set_cs(true);
}

uint8_t ADXL345::readRegister(uint8_t reg) {
    uint8_t tx[2] = { static_cast<uint8_t>(0x80 | reg), 0x00 };
    uint8_t rx[2]{};

    struct spi_ioc_transfer tr{};
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 2;
    tr.speed_hz = speed_hz;
    tr.bits_per_word = 8;

    set_cs(false);
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        throw runtime_error("SPI transfer failed");
    }
    set_cs(true);

    return rx[1];
}

int16_t ADXL345::readWord(uint8_t reg) {
    uint8_t tx[3] = { static_cast<uint8_t>(0x80 | 0x40 | reg), 0x00, 0x00 };
    uint8_t rx[3]{};

    struct spi_ioc_transfer tr{};
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 3;
    tr.speed_hz = speed_hz;
    tr.bits_per_word = 8;

    set_cs(false);
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        throw runtime_error("SPI transfer failed");
    }
    set_cs(true);

    return static_cast<int16_t>((rx[2] << 8) | rx[1]);
}

void ADXL345::readXYZ(float& x, float& y, float& z) {
    int16_t rawX = readWord(0x32);
    int16_t rawY = readWord(0x34);
    int16_t rawZ = readWord(0x36);

    // Full resolution: 3.9 mg/LSB
    constexpr float scale = 0.0039f;

    x = rawX * scale;
    y = rawY * scale;
    z = rawZ * scale;
}

int8_t ADXL345::calc_offset_reg(float measured_g)
{
    // We want to cancel the measured offset
    // so write the NEGATIVE value
    int offset = (int)lround(-measured_g / OFFSET_G_PER_LSB);

    // Clamp to int8_t range
    if (offset > 127)  offset = 127;
    if (offset < -128) offset = -128;

    return (int8_t)offset;
}

void ADXL345::adxl345_write_offsets(float x_g, float y_g, float z_g)
{
    int8_t ofs_x = calc_offset_reg(x_g);
    int8_t ofs_y = calc_offset_reg(y_g);

    // For Z, expect +1 g at rest
    int8_t ofs_z = calc_offset_reg(z_g - 1.0);

    printf("2nd write offset XYZ\n");
    printf("X: 0x%02X\n", (uint8_t)ofs_x);
    printf("Y: 0x%02X\n", (uint8_t)ofs_y);
    printf("Z: 0x%02X\n", (uint8_t)ofs_z);
    writeRegister(ADXL345_OFSX, (uint8_t)ofs_x);
    sleep(1);
    writeRegister(ADXL345_OFSY, (uint8_t)ofs_y);
    sleep(1);
    writeRegister(ADXL345_OFSZ, (uint8_t)ofs_z);
    sleep(1);
}

void ADXL345::offsetreg_value()
{
        uint8_t xb, yb, zb;
        writeRegister(ADXL345_OFSX, (uint8_t)0x0);
        sleep(1);
        writeRegister(ADXL345_OFSY, (uint8_t)0x0);
        sleep(1);
        writeRegister(ADXL345_OFSZ, (uint8_t)0x0);
        sleep(1);
        xb = readRegister(ADXL345_OFSX);
        sleep(1);
        yb = readRegister(ADXL345_OFSY);
        sleep(1);
        zb = readRegister(ADXL345_OFSZ);
        printf("1st read offset XYZ\n");
        printf("X: 0x%02X\n", xb);
        printf("Y: 0x%02X\n", yb);
        printf("Z: 0x%02X\n", zb);
}

void ADXL345::collect_xyz_data()
{
    float x, y, z;

    readXYZ(x, y, z);
    // printf("x:%.4f, y:%.4f, z:%.4f\n", x, y, z)

    vx.push_back(x);
    vy.push_back(y);
    vz.push_back(z);

    // Keep only the last MAX_POINTS
    if (vx.size() > MAX_POINTS)
    {
        vx.erase(vx.begin());
        vy.erase(vy.begin());
        vz.erase(vz.begin());
    }
}

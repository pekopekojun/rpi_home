#ifndef __INA260_H__
#define __INA260_H__

#include "Arduino.h"

#define INA260_DEFAULT_ADDR (0x40)

// Operating Mode (Mode Settings [2:0])
#define INA260_CONF_MODE_CONT_SHUNT_AND_BUS \
  (7 << 0)  // Shunt Current and Bus Voltage, Continuous (default)

// Shunt Current Conversion Time (SHCT CT Bit Settings [5:3])
#define INA260_CONF_SHCT_140uS (0 << 3)   // 140us
#define INA260_CONF_SHCT_204uS (1 << 3)   // 204us
#define INA260_CONF_SHCT_332uS (2 << 3)   // 332us
#define INA260_CONF_SHCT_588uS (3 << 3)   // 588us
#define INA260_CONF_SHCT_1100uS (4 << 3)  // 1.1ms (default)
#define INA260_CONF_SHCT_2116uS (5 << 3)  // 2.116ms
#define INA260_CONF_SHCT_4156uS (6 << 3)  // 4.156ms
#define INA260_CONF_SHCT_8244uS (7 << 3)  // 8.244ms

// Bus Voltage Conversion Time (VBUSCT CT Bit Settings [8:6])
#define INA260_CONF_VBUSCT_140uS (0 << 6)   // 140us
#define INA260_CONF_VBUSCT_204uS (1 << 6)   // 204us
#define INA260_CONF_VBUSCT_332uS (2 << 6)   // 332us
#define INA260_CONF_VBUSCT_588uS (3 << 6)   // 588us
#define INA260_CONF_VBUSCT_1100uS (4 << 6)  // 1.1ms (default)
#define INA260_CONF_VBUSCT_2116uS (5 << 6)  // 2.116ms
#define INA260_CONF_VBUSCT_4156uS (6 << 6)  // 4.156ms
#define INA260_CONF_VBUSCT_8244uS (7 << 6)  // 8.244ms

// Averaging Mode (AVG Bit Settings[11:9])
#define INA260_CONF_AVG_1 (0 << 9)     // 1 (default)
#define INA260_CONF_AVG_4 (1 << 9)     // 4
#define INA260_CONF_AVG_16 (2 << 9)    // 16
#define INA260_CONF_AVG_64 (3 << 9)    // 64
#define INA260_CONF_AVG_128 (4 << 9)   // 128
#define INA260_CONF_AVG_256 (5 << 9)   // 256
#define INA260_CONF_AVG_512 (6 << 9)   // 512
#define INA260_CONF_AVG_1024 (7 << 9)  // 1024

// Reset Bit (RST bit [15])
#define INA260_CONF_RESET_ACTIVE (1 << 15)
#define INA260_CONF_RESET_INACTIVE (0 << 15)

class INA260 {
  private:
    int s_addr;

    void writeRegister(uint8_t reg, uint16_t value);
    uint16_t readRegister(uint8_t reg);
    long conv_sign(int reg);
  public:
    INA260(int8_t saddr = INA260_DEFAULT_ADDR);
    void     setup();
    uint16_t getRawCurrent();
    uint16_t getRawVoltage();
    uint16_t getRawPworer();
    long     getCurrent();
    long     getVoltage();
    long     getPworer();
};
#endif /* __INA260_H__ */

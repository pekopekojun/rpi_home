#ifndef __INA226_H__
#define __INA226_H__

#include "Arduino.h"

#define INA226_DEFAULT_ADDR (0x40)
#define INA226_CAL_VALUE (205)

// Operating Mode (Mode Settings [2:0])
#define INA226_CONF_MODE_POWER_DOWN (0 << 0)          // Power-Down
#define INA226_CONF_MODE_TRIG_SHUNT_VOLTAGE (1 << 0)  // Shunt Voltage, Triggered
#define INA226_CONF_MODE_TRIG_BUS_VOLTAGE (2 << 0)    // Bus Voltage, Triggered
#define INA226_CONF_MODE_TRIG_SHUNT_AND_BUS (3 << 0)  // Shunt and Bus, Triggered
#define INA226_CONF_MODE_POWER_DOWN2 (4 << 0)         // Power-Down
#define INA226_CONF_MODE_CONT_SHUNT_VOLTAGE (5 << 0)  // Shunt Voltage, Continuous
#define INA226_CONF_MODE_CONT_BUS_VOLTAGE (6 << 0)    // Bus Voltage, Continuous
#define INA226_CONF_MODE_CONT_SHUNT_AND_BUS (7 << 0)  // Shunt and Bus, Continuous (default)

// Shunt Voltage Conversion Time (VSH CT Bit Settings [5:3])
#define INA226_CONF_VSH_140uS (0 << 3)   // 140us
#define INA226_CONF_VSH_204uS (1 << 3)   // 204us
#define INA226_CONF_VSH_332uS (2 << 3)   // 332us
#define INA226_CONF_VSH_588uS (3 << 3)   // 588us
#define INA226_CONF_VSH_1100uS (4 << 3)  // 1.1ms (default)
#define INA226_CONF_VSH_2116uS (5 << 3)  // 2.116ms
#define INA226_CONF_VSH_4156uS (6 << 3)  // 4.156ms
#define INA226_CONF_VSH_8244uS (7 << 3)  // 8.244ms

// Bus Voltage Conversion Time (VBUS CT Bit Settings [8:6])
#define INA226_CONF_VBUS_140uS (0 << 6)   // 140us
#define INA226_CONF_VBUS_204uS (1 << 6)   // 204us
#define INA226_CONF_VBUS_332uS (2 << 6)   // 332us
#define INA226_CONF_VBUS_588uS (3 << 6)   // 588us
#define INA226_CONF_VBUS_1100uS (4 << 6)  // 1.1ms (default)
#define INA226_CONF_VBUS_2116uS (5 << 6)  // 2.116ms
#define INA226_CONF_VBUS_4156uS (6 << 6)  // 4.156ms
#define INA226_CONF_VBUS_8244uS (7 << 6)  // 8.244ms

// Averaging Mode (AVG Bit Settings[11:9])
#define INA226_CONF_AVG_1 (0 << 9)     // 1 (default)
#define INA226_CONF_AVG_4 (1 << 9)     // 4
#define INA226_CONF_AVG_16 (2 << 9)    // 16
#define INA226_CONF_AVG_64 (3 << 9)    // 64
#define INA226_CONF_AVG_128 (4 << 9)   // 128
#define INA226_CONF_AVG_256 (5 << 9)   // 256
#define INA226_CONF_AVG_512 (6 << 9)   // 512
#define INA226_CONF_AVG_1024 (7 << 9)  // 1024

// Reset Bit (RST bit [15])
#define INA226_CONF_RESET_ACTIVE (1 << 15)
#define INA226_CONF_RESET_INACTIVE (0 << 15)

class INA226 {
  private:
    long s_current_lsb_ua = 100L;    // LSB=0.1mA
    long s_voltage_lsb_uv = 1250L;   // LSB=1.25mV
    long s_power_lsb_uw   = 25000L;  // LSB=25mW
    int s_addr;

    void     writeRegister(uint8_t reg, uint16_t value);
    uint16_t readRegister(uint8_t reg);
    long     conv_sign(int reg);

  public:
    INA226(int8_t saddr = INA226_DEFAULT_ADDR);
    void setup();
    void setCalibration(uint16_t cal);
    void setLSB(long current_lsb_ua, long voltage_lsb_uv, long power_lsb_uw);

    uint16_t getRawShuntVoltage();
    uint16_t getRawCurrent();
    uint16_t getRawVoltage();
    uint16_t getRawPworer();

    long getCurrentFromShuntVoltage();
    long getCurrent();
    long getVoltage();
    long getPworer();
};

#endif /* __INA226_H__ */

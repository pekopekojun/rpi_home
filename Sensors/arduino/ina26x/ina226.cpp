#include "ina226.h"
#include <Wire.h>

// INA226 Registers
#define INA226_REG_CONGIGURATION 0x00  // Configuration Register (R/W)
#define INA226_REG_SHUNT_VOLTAGE 0x01  // Shunt Voltage (R)
#define INA226_REG_BUS_VOLTAGE 0x02    // Bus Voltage (R)
#define INA226_REG_POWER 0x03          // Power (R)
#define INA226_REG_CURRENT 0x04        // Current (R)
#define INA226_REG_CALIBRATION 0x05    // Calibration (R/W)
#define INA226_REG_MASK_ENABLE 0x06    // Mask/Enable (R/W)
#define INA226_REG_ALERT_LIMIT 0x07    // Alert Limit (R/W)
#define INA226_REG_DIE_ID 0xFF         // Die ID (R)

INA226::INA226(int8_t saddr ) {
  s_addr = saddr;
}

void INA226::writeRegister(uint8_t reg, uint16_t value)
{
  Wire.beginTransmission(s_addr);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

uint16_t INA226::readRegister(uint8_t reg)
{
  uint16_t res = 0x0000;

  Wire.beginTransmission(s_addr);
  Wire.write(reg);

  if (Wire.endTransmission() == 0) {
    if (Wire.requestFrom(s_addr, 2) >= 2) {
      res = Wire.read() * 256;
      res += Wire.read();
    }
  }

  return res;
}

long INA226::conv_sign(int reg)
{
  long reg_tmp = reg;

  if ((reg_tmp & 0x8000UL) != 0) {
    reg_tmp = (~reg_tmp) & 0x7fff;
    reg_tmp++;
    reg_tmp *= -1;
  }
  return reg_tmp;
}

void INA226::setCalibration(uint16_t cal)
{
  writeRegister(INA226_REG_CALIBRATION, cal);
}

void INA226::setLSB(long current_lsb_ua, long voltage_lsb_uv, long power_lsb_uw)
{
  s_current_lsb_ua = current_lsb_ua;
  s_voltage_lsb_uv = voltage_lsb_uv;
  s_power_lsb_uw   = power_lsb_uw;
}

uint16_t INA226::getRawShuntVoltage()
{
  return (uint16_t)readRegister(INA226_REG_SHUNT_VOLTAGE);
}

uint16_t INA226::getRawCurrent()
{
  return (uint16_t)readRegister(INA226_REG_CURRENT);
}

uint16_t INA226::getRawVoltage()
{
  return (uint16_t)readRegister(INA226_REG_BUS_VOLTAGE);
}

uint16_t INA226::getRawPworer()
{
  return (uint16_t)readRegister(INA226_REG_POWER);
}

long INA226::getCurrentFromShuntVoltage()
{
  return conv_sign(getRawShuntVoltage()) * s_current_lsb_ua;
}

long INA226::getCurrent()
{
  return conv_sign(getRawCurrent()) /** s_current_lsb_ua*/;
}

long INA226::getVoltage()
{
  return getRawVoltage() * s_voltage_lsb_uv;
}

long INA226::getPworer()
{
  return conv_sign(getRawPworer()) * s_power_lsb_uw;
}

void INA226::setup()
{
  writeRegister(INA226_REG_CONGIGURATION,
                INA226_CONF_RESET_INACTIVE | INA226_CONF_MODE_CONT_SHUNT_AND_BUS |
                INA226_CONF_VSH_1100uS | INA226_CONF_VBUS_1100uS | INA226_CONF_AVG_64);
}

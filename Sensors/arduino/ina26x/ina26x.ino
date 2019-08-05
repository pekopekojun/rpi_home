/*********************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x64 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Dialog_plain_32Bitmaps.h"
#include "Dialog_plain_16Bitmaps.h"
#include <math.h>
#include <EEPROM.h>


#include "ina260.h"
#include "ina226.h"

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


INA226 ina226(0x40);
INA260 ina260(0x44);

static int8_t s_ina260_current_ofs_100ua = 37;
static int8_t s_ina260_voltage_ofs_100uv = 0;

void setup()   {
  int cal;
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.setTextColor(WHITE);
  ina260.setup();
  ina226.setup();

  cal = EEPROM.read(0);
  if (cal == 0 || cal == 255) {
    cal = INA226_CAL_VALUE;
  }

  s_ina260_current_ofs_100ua = (int8_t)EEPROM.read(1);
  s_ina260_voltage_ofs_100uv = (int8_t)EEPROM.read(2);

  Serial.print("CALIBRATION Register :");
  Serial.println(cal, DEC);
  Serial.print(">");
  ina226.setCalibration(cal);
  Serial.println("");

  display.clearDisplay();
  display.setFont(&Dialog_plain_16);
  display.setCursor(0, 15);
  display.print("226 Cal :"); display.println(cal);
  display.print("260 Aofs:"); display.println(s_ina260_current_ofs_100ua);
  display.display();
  delay(3000);
}


void loop() {

  long uv;
  long ua;
  long uw;
  long ina226_uv;
  long ina226_ma;
  long ina226_uw;

  unsigned long base_time = millis();

  uv = ina260.getVoltage();
  ua = ina260.getCurrent() - (s_ina260_current_ofs_100ua * 100);
  uw = ina260.getPworer();

  ina226_uv = ina226.getVoltage();
  ina226_ma = ina226.getCurrent();
  ina226_uw = ina226.getPworer();

  if (Serial.available() > 0)
  {
    int32_t value;
    float f_cal;
    int cal;
    String str = "";
    str = Serial.readStringUntil('\n');
    value =  (int32_t)atoi((char *)str.c_str());
    f_cal = ((float)value * 100.0f) / (float)ina226.getCurrentFromShuntVoltage();
    cal = round(INA226_CAL_VALUE * f_cal);
    Serial.println(value, DEC); ;

    Serial.print(">");
    Serial.print(ina226_ma, DEC); ;
    Serial.print("mA ->");

    Serial.print(value / 10, DEC);
    Serial.print(".");
    Serial.print(value % 10, DEC);
    Serial.println("mA");

    Serial.print(">CALIBRATION Register:");
    Serial.print(INA226_CAL_VALUE, DEC); ;
    Serial.print("=>");
    Serial.println(cal, DEC); ;
    Serial.println(">Calibration OK!!!");

    ina226.setCalibration(cal);
    EEPROM.write(0, cal);

    /* ina260  */
    s_ina260_current_ofs_100ua = (ina260.getCurrent() - (value * 100)) / 100;
    EEPROM.write(1, s_ina260_current_ofs_100ua);

    Serial.println(s_ina260_current_ofs_100ua, DEC); ;
    Serial.println(">Calibration OK!!!");

    EEPROM.write(2, 0); // voltage
    while (millis() - base_time < 3000) {
      ;
    }
  } else {

    display.clearDisplay();
    display.setFont(&Dialog_plain_16);
    display.setCursor(0, 15);

    display.print((float)uv / 1000000.0f, 1); display.print(",");
    display.print(ua / 1000, DEC); display.print(",");
    float mw =  (uv / 1000000.0f) * (ua / 1000.0f);
    display.print(mw / 1000.0f, 2); display.println("");

    display.print((float)ina226_uv / 1000000.0f, 1); display.print(",");
    display.print(ina226_ma, DEC); display.print(",");
    float ina226_mw = (ina226_uv / 1000000.0f) * ina226_ma;
    display.print(ina226_mw, 1);  display.println("");

    display.print((ina226_mw / mw) * 100, 2);

    display.display();
    /*
        Serial.print(uv / 1000.0f, 2); ;
        Serial.print("mV,");
        Serial.print(ua / 1000.0f, 2); ;
        Serial.print("mA,");
        Serial.print(uw / 1000.0f, 2); ;
        Serial.print("mW,");
        Serial.print(ina226_uv / 1000.0f, 2); ;
        Serial.print("mV,");
        Serial.print(ina226_ma / 1.0f , 2); ;
        Serial.print("mA,");
        Serial.print((ina226_uv / 1000000.0f)*ina226_ma, 2); ;
        Serial.print("mW,");
        Serial.println("");*/
  }
  while (millis() - base_time < 100) {
    ;
  }
}



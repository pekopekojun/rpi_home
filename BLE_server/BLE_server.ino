/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Wire.h>
#include "Adafruit_ADT7410.h"


// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();


BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

char adv_packet[] = {0xe0, 0x00, /*00-01 Company ID : Google */
                     0x00,       /*02    Beacon Number */
                     0x00,       /*03    Error Code (reserved)*/
                     0x00,       /*04    Battery type: 0x00:wired supply
                                                       0x01:Li-ion ptrn1
                                                       0x02:Li-ion ptrn2 */
                     0x00, 0x00, /*05-06 remaining battery
                                        Battery type: 0x00 then always 0x0000
                                                      0x01 then [mV]
                                                      0x02 then [%]*/
                     0x01,       /*07    Enable Sensor: bit0:Temp
                                                        bit1:Hum
                                                        bit2:Press
                                                        bit3:Lum*/
                     0x00, 0x00, /*08-09 Temp  Value: signed short [0.01 degree]*/
                     0x00,       /*10    Hum   Value: unsigned char [%]*/
                     0x00, 0x00, /*11-12 Press Value: unsigned short [hPa]*/
                     0x00, 0x00, /*13-14 Lum   Value: unsigned short [lux] */
                    };

// M5 Stack
int get_battery_gauge_for_m5stack(void)
{
  int reg = 0xff;
  Wire.beginTransmission(0x75);
  Wire.write(0x78);
  Wire.endTransmission(false);
  if (Wire.requestFrom(0x75, 1)) {
    reg = Wire.read();
    return reg; /*bit7: 75%, bit6:50% bit5:25% bit4: Below 3.2v */
  } else {
    return -1;
  }
}

void set_battery_type(uint8_t type)
{
  adv_packet[4] = type;
}

float get_battery_mv_for_li_ion(void)
{
  int ad = analogRead(A5); // silk 33
  float raw_v = ((long)ad * 3.3f * 1000) / 4096;
  float conv_v = raw_v * (4720.0 / 3040.0);
  return conv_v;
}

void set_remaining_battery(uint16_t mv)
{
  adv_packet[5] = mv & 0xff;
  adv_packet[6] = (mv >> 8) & 0xff;
}

void set_temp(int16_t v)
{
  adv_packet[8] = v & 0xff;
  adv_packet[9] = (v >> 8) & 0xff;
}



void update_adv(void)
{
  BLEAdvertisementData *adv_data = new BLEAdvertisementData;
  adv_data->setName("SensorBeacon");
  adv_data->setManufacturerData(std::string(adv_packet, sizeof(adv_packet)));
  pAdvertising->setAdvertisementData(*adv_data);
}

void setup() {
  int start;
  //Serial.begin(115200);

  if (!tempsensor.begin()) {
    //  Serial.println("Couldn't find ADT7410!");
  }
  //Wait 240ms
  start = millis();
  //  tempsensor.write8(ADT7410_REG__ADT7410_CONFIG, 0x40); //1 SPS MODE

  BLEDevice::init("");

  /* 0.625ms*/
  /* 1Sec*/
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  //pAdvertising->setMinInterval(0x0640);
  //pAdvertising->setMaxInterval(0x0640);
  //200ms
  pAdvertising->setMinInterval(320);
  pAdvertising->setMaxInterval(320);

  int diff = (millis() - start);
  if (diff < 240) {
    delay(240 - diff + 10 /* 10 is margin */);
  }

  int16_t c = (int16_t)(tempsensor.readTempC() * 100U);
  uint16_t b = (uint16_t)get_battery_mv_for_li_ion();
  set_battery_type(0x01);
  set_remaining_battery(b);
  set_temp(c);
  
  update_adv();

  pAdvertising->start();
  //  Serial.println("Start Advertising");
}


void loop() {
  /* Adv interval 200ms * 15 times (3Sec), DeepSleep timer is 177Sec*/
  delay(3000);

  esp_sleep_enable_timer_wakeup(177 * 1000 * 1000);  // wakeup every 5secs
//  Serial.println("zzz...");
  esp_deep_sleep_start();
/*
  int16_t c = (int16_t)(tempsensor.readTempC() * 100U);
  uint16_t b = (uint16_t)get_battery_mv_for_li_ion();
  set_battery_type(0x01);
  set_remaining_battery(b);
  set_temp(c);

  Serial.print("Temp: "); Serial.print(c); Serial.println("*C");
  set_temp(c);
  update_adv();
  Serial.print (b);
  Serial.println ("");
  delay(100);
*/
}

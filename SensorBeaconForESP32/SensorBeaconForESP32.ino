/**
 * BLE Sensor Beacon for ESP32
 *  
 * Copyright (c) 2019 Jun Sato (peko_jun)
 * Released under the MIT license.
 * see https://opensource.org/licenses/MIT
 */

/*
 * Supporting sensors
 *  - Temperature sensor  
 *    - Hardware : ADT7410 (http://akizukidenshi.com/catalog/g/gM-06675/)
 *    - Library :  Adafruit_ADT7410 (https://github.com/adafruit/Adafruit_ADT7410)
 *
 */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Wire.h>
#include "Adafruit_ADT7410.h"

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

void set_battery_type(uint8_t type)
{
  adv_packet[4] = type;
}

float get_battery_mv_for_li_ion(void)
{
  // TODO: Not implemented properly
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
}

#define ADV_TIME_MS (3000)
#define DEEP_SLEEP_TIME_MS ((250 * 1000) - ADV_TIME_MS)

void loop() {
  delay(ADV_TIME_MS);

  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_MS * 1000);  
  esp_deep_sleep_start();
}

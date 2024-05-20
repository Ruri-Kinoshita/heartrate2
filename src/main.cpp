/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "M5StickCPlus2.h"

#define REPORTING_PERIOD_MS 500

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation５
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0;

uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue)
{
  return ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
}

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  M5.begin();
  M5.Lcd.setRotation(3); // 　画面向きを設定

  // ハートを表示
  M5.Lcd.fillCircle(30, 40, 10, RED);
  M5.Lcd.fillCircle(50, 40, 10, RED);
  M5.Lcd.fillCircle(40, 41, 3, RED);
  M5.Lcd.fillTriangle(22, 45, 58, 45, 40, 65, RED);

  // O2を表示
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(20, 80);
  M5.Lcd.print("O");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(48, 100);
  M5.Lcd.print("2");

  Serial.begin(115200);

  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;)
      ;
  }
  else
  {
    Serial.println("SUCCESS");
  }

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
  // Make sure to call update as fast as possible
  pox.update();

  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    M5.Lcd.setTextSize(3);

    // 心拍数を表示
    M5.Lcd.setCursor(75, 40);
    M5.Lcd.print(pox.getHeartRate());

    // 　血中酸素量を表示
    M5.Lcd.setCursor(75, 90);
    M5.Lcd.print(pox.getSpO2());

    tsLastReport = millis();
  }
}
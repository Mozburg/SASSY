#include <Arduino.h>
/* This example shows how to use continuous mode to take
 * range measurements with the VL53L0X. It is based on
 * vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.
 *
 * The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>
#include <ESP8266WiFi.h>
///bipbopboop///

VL53L0X sensor;

#define HIGH_SPEED
//#define HIGH_ACCURACY


void setup()
{
   Serial.begin(9600);
   Wire.begin();

   sensor.setTimeout(500);
   if (!sensor.init())
   {
      Serial.println("Failed to detect and initialize sensor!");
      while (1)
      {
      }
   }

#if defined LONG_RANGE
   // lower the return signal rate limit (default is 0.25 MCPS)
   sensor.setSignalRateLimit(0.1);
   // increase laser pulse periods (defaults are 14 and 10 PCLKs)
   sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
   sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
   // reduce timing budget to 20 ms (default is about 33 ms)
   sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
   // increase timing budget to 200 ms
   sensor.setMeasurementTimingBudget(200000);
#endif
}

void loop()
{
   Serial.println(sensor.readRangeSingleMillimeters());
   if (sensor.timeoutOccurred())
   {
      Serial.print(" TIMEOUT");
   }


   // Serial.println();
}

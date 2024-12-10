#include "Adafruit_PM25AQI.h"
#include <HardwareSerial.h>

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
HardwareSerial MySerial0(0);

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println("Adafruit PMSA003I Air Quality Sensor");
  delay(3000);
  
MySerial0.begin(9600, SERIAL_8N1, 8, 5);
if (! aqi.begin_UART(&MySerial0)) { 
     Serial.println("Could not find PM 2.5 sensor!");
     while (1) delay(10);
   }
  Serial.println("Sensor found!");
}

void loop() {
PM25_AQI_Data data;
if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);
    return;
}
  Serial.print("data: ");
  Serial.print(data.pm10_standard); Serial.print(" | ");
  Serial.print(data.pm25_standard); Serial.print(" | ");
  Serial.print(data.pm100_standard); Serial.print(" | ");
  Serial.print(data.particles_03um); Serial.print(" | ");
  Serial.print(data.particles_05um); Serial.print(" | ");
  Serial.println(data.particles_10um);
  delay(4000);
}

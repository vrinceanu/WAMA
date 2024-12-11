#include "Adafruit_PM25AQI.h"
#include <HardwareSerial.h>
#include "DHT.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
HardwareSerial MySerial0(0);
DHT dht(D10, DHT11);
BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;

int temp, humi, pm03, pm05, pm1, pm25, pm50, pm100;
bool deviceConnected = false;
bool oldDeviceConnected = false;

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }
      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup() {
  Serial.begin(115200);
  dht.begin();
  MySerial0.begin(9600, SERIAL_8N1, 8, 5);
  if (! aqi.begin_UART(&MySerial0)) { 
       Serial.println("Could not find PM 2.5 sensor!");
       while (1) delay(10);
   }
  Serial.println("PM Sensor found!");

  BLEDevice::init("UART Service");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");  
  delay(1000);
}

void loop() {
 char payload[47];
  PM25_AQI_Data data;
  if (! aqi.read(&data)) {
      Serial.println("Could not read from AQI");
      delay(2000);
  }
  humi = int(100*dht.readHumidity());
  temp = int(100*dht.readTemperature());
  pm03 = data.particles_03um;
  pm05 = data.particles_05um;
  pm1  = data.particles_10um;
  pm25 = data.particles_25um;
  pm50 = data.particles_50um;
  pm100 = data.particles_100um;
 
 // 0000,1111,22222,33333,44444,55555,66666,77777n
  itoa(temp,payload,10); strcat(payload,",");
  itoa(humi,payload+strlen(payload),10); strcat(payload,",");
  itoa(pm03,payload+strlen(payload),10); strcat(payload,",");
  itoa(pm05,payload+strlen(payload),10); strcat(payload,",");
  itoa(pm1,payload+strlen(payload),10); strcat(payload,",");
  itoa(pm25,payload+strlen(payload),10); strcat(payload,",");
  itoa(pm50,payload+strlen(payload),10); strcat(payload,",");
  itoa(pm100,payload+strlen(payload),10); strcat(payload,"\n");

  if (deviceConnected) {
    pTxCharacteristic->setValue((uint8_t*) payload, strlen(payload));
    pTxCharacteristic->notify();
    Serial.print(temp); Serial.print(" | ");
    Serial.print(humi); Serial.print(" | ");
    Serial.print(pm03); Serial.print(" | ");
    Serial.print(pm05); Serial.print(" | ");
    Serial.print(pm1); Serial.print(" | ");
    Serial.print(pm25); Serial.print(" | ");
    Serial.print(pm50); Serial.print(" | ");   
    Serial.println(pm100);
    Serial.print(payload);
    delay(2000);
  }

// disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
// connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}

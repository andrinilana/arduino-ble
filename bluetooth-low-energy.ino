#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs arbitraires (tu peux les changer)
#define SERVICE_UUID "b3b83c9e-8c0b-4cb2-b2f5-3d204a2f2f9a"
#define CHARACTERISTIC_UUID "a0c17ef1-f043-4c41-8c89-1a2b3e6f7d52"

BLECharacteristic *pCharacteristic;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue().c_str();  // Convert to Arduino String

    if (rxValue.length() > 0) {
      Serial.print("Received: ");
      Serial.println(rxValue);
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Init BLE
  BLEDevice::init("ESP32_BLE");
  BLEServer *pServer = BLEDevice::createServer();

  // Crée un service BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Crée une caractéristique BLE
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID, 
    BLECharacteristic::PROPERTY_WRITE | 
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Ajoute un descripteur pour les notifications
  pCharacteristic->addDescriptor(new BLE2902());

  // Démarre le service
  pService->start();

  // Commence la publicité BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("BLE service is up, waiting for connections...");
}

void loop() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 1000) {    
    lastSend = millis();
    String msg = "t=90;f=45";
    pCharacteristic->setValue(msg);
    pCharacteristic->notify();  // Envoie via notification BLE
    Serial.println("Sent: " + msg);
  }
}

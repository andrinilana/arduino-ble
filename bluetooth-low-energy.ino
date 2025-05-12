#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs arbitraires (tu peux les changer)
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcd1234-ab12-cd34-ef56-1234567890ab"

BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);

  // Init BLE
  BLEDevice::init("ESP32_BLE");
  BLEServer *pServer = BLEDevice::createServer();

  // Crée un service BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Crée une caractéristique BLE
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

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
    String msg = "temperature=90";
    lastSend = millis();
    pCharacteristic->setValue(msg);
    pCharacteristic->notify();  // Envoie via notification BLE
    Serial.println("Sent: " + msg);
  }
}

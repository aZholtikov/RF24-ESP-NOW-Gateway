#include "nRF24L01.h"
#include "RF24.h"
#include "ArduinoJson.h"
#include "Ticker.h"
#include "ZHNetwork.h"
#include "ZHSmartHomeProtocol.h"
#include "ZHRF24SensorProtocol.h"

//***********************КОНФИГУРАЦИЯ***********************//
// Настройки модуля RF24:
#define NRF24_CSN 15
#define NRF24_CE 4

// Настройки радиоканала:
#define PIPE 0xDDEEFF
#define CHANNEL 120

const char *myNetName{"SMART"}; // Укажите имя сети ESP-NOW.
//***********************************************************//

void onBroadcastReceiving(const char *data, const byte *sender);
void onUnicastReceiving(const char *data, const byte *sender);
void restart(void);
void attributesMessage(void);
void keepAliveMessage(void);

String firmware{"1.0"};
byte gatewayMAC[6]{0};

ZHNetwork myNet;
RF24 radio(NRF24_CE, NRF24_CSN);
Ticker attributesMessageTimer;
Ticker keepAliveMessageTimer;
Ticker restartTimer;

void setup()
{
  myNet.begin(myNetName);
  myNet.setOnBroadcastReceivingCallback(onBroadcastReceiving);
  myNet.setOnUnicastReceivingCallback(onUnicastReceiving);
  radio.begin();
  radio.setChannel(CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(14);
  radio.setAddressWidth(3);
  radio.setCRCLength(RF24_CRC_8);
  radio.openReadingPipe(0, PIPE);
  radio.startListening();
}

void loop()
{
  myNet.maintenance();
  if (radio.available())
  {
    TransmittedData receivedData;
    radio.read(&receivedData, sizeof(struct TransmittedData));
    PayloadsData outgoingData{RF24_GATEWAY, STATE};
    StaticJsonDocument<sizeof(outgoingData.message)> json;
    char buffer[sizeof(outgoingData.message)];
    if (receivedData.sensor_type == BME280)
    {
      json["humidity"] = receivedData.value_2;
      json["temperature"] = receivedData.value_3;
      json["pressure"] = receivedData.value_4;
    }
    if (receivedData.sensor_type == BMP280)
    {
      json["temperature"] = receivedData.value_2;
      json["pressure"] = receivedData.value_3;
    }
    if (receivedData.sensor_type == BME680)
    {
      json["humidity"] = receivedData.value_2;
      json["temperature"] = receivedData.value_3;
      json["pressure"] = receivedData.value_4;
      json["quality"] = receivedData.value_5;
    }
    if (receivedData.sensor_type == WATER_LEAKAGE)
      json["state"] = receivedData.value_2 == ALARM ? "ALARM" : "DRY";
    if (receivedData.sensor_type == PLANT_HUMIDITY)
      json["humidity"] = receivedData.value_2;
    if (receivedData.sensor_type == OPEN_CLOSE)
      json["state"] = receivedData.value_2 == OPEN ? "OPEN" : "CLOSE";
    json["type"] = receivedData.sensor_type;
    json["id"] = receivedData.sensor_id;
    json["battery"] = double(receivedData.value_1) / 100;
    serializeJsonPretty(json, buffer);
    os_memcpy(outgoingData.message, buffer, sizeof(outgoingData.message));
    char temp[sizeof(PayloadsData)];
    os_memcpy(temp, &outgoingData, sizeof(PayloadsData));
    myNet.sendUnicastMessage(temp, gatewayMAC);
  }
}

void onBroadcastReceiving(const char *data, const byte *sender)
{
  PayloadsData incomingData;
  os_memcpy(&incomingData, data, sizeof(incomingData));
  if (incomingData.deviceType != GATEWAY || myNet.macToString(gatewayMAC) == myNet.macToString(sender))
    return;
  if (incomingData.payloadsType == KEEP_ALIVE)
  {
    os_memcpy(gatewayMAC, sender, 6);
    attributesMessage();
    keepAliveMessage();
    attributesMessageTimer.attach(3600, attributesMessage);
    keepAliveMessageTimer.attach(60, keepAliveMessage);
  }
}

void onUnicastReceiving(const char *data, const byte *sender)
{
  PayloadsData incomingData;
  os_memcpy(&incomingData, data, sizeof(incomingData));
  if (incomingData.deviceType != GATEWAY || myNet.macToString(gatewayMAC) != myNet.macToString(sender))
    return;
  if (incomingData.payloadsType == UPDATE)
  {
    radio.stopListening();
    myNet.update();
    attributesMessageTimer.detach();
    keepAliveMessageTimer.detach();
    restartTimer.once(300, restart);
  }
  if (incomingData.payloadsType == RESTART)
    restart();
}

void restart(void)
{
  ESP.restart();
}

void attributesMessage()
{
  PayloadsData outgoingData{RF24_GATEWAY, ATTRIBUTES};
  StaticJsonDocument<sizeof(outgoingData.message)> json;
  json["MCU"] = "ESP8266";
  json["MAC"] = myNet.getNodeMac();
  json["Firmware"] = firmware;
  json["Library"] = myNet.getFirmwareVersion();
  char buffer[sizeof(outgoingData.message)];
  serializeJsonPretty(json, buffer);
  os_memcpy(outgoingData.message, buffer, sizeof(outgoingData.message));
  char temp[sizeof(PayloadsData)];
  os_memcpy(temp, &outgoingData, sizeof(PayloadsData));
  myNet.sendUnicastMessage(temp, gatewayMAC);
}

void keepAliveMessage()
{
  PayloadsData outgoingData{RF24_GATEWAY, KEEP_ALIVE};
  char temp[sizeof(PayloadsData)];
  os_memcpy(temp, &outgoingData, sizeof(PayloadsData));
  myNet.sendUnicastMessage(temp, gatewayMAC);
}

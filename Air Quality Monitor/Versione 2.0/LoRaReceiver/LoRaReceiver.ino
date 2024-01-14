#include <LoRa.h>
#include <SPI.h>

#include <WiFi.h>
#include "EspMQTTClient.h"
//#include <PubSubClient.h>

#define ss 5
#define rst 14
#define dio0 2


EspMQTTClient client(
  "********",
  "********",
  "*********",  // MQTT Broker server ip
  "ESP32",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

const char *topic = "esp32/qualutyaircheck99";
const char *topic_setting = "esp32/qualutyaircheck99_setting";


void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");
 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
 
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

}

void loop() {
  client.loop();
  int packetSize = LoRa.parsePacket();
  String LoRaData = "";
  if (packetSize) {
    //received a packet
    Serial.println("Received packet\n ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      int str_len = LoRaData.length() + 1;
      char char_array[str_len];
      LoRaData.toCharArray(char_array, str_len);
      //Serial.println(client.state());
      client.publish(topic,char_array);
    }
  }
  
}

void onConnectionEstablished()
{

    client.publish(topic_setting, "Connesso!!!");

}

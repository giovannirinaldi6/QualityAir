#include <LoRa.h>
#include <SPI.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define ss 5
#define rst 14
#define dio0 2

const char* ssid = "*********";
const char* password = "******";

// MQTT Broker
   //const char *mqtt_broker = "test.mosquitto.org";
   const char *mqtt_broker = "************";
   const char *topic = "esp32/qualutyaircheck99";
   const char *topic_setting = "esp32/qualutyaircheck99_setting";
   const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

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

  setup_wifi();

  client.setServer(mqtt_broker, mqtt_port);

  while (!client.connected()) {
  reconnect();
 }

 Serial.print("Connesso");
 // publish
 client.publish(topic_setting, "Hi EMQX I'm ESP32 ^^");
}
 
void loop() 
{
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
      Serial.println(client.state());
      client.publish(topic,char_array);
    }
  }
  
}



void setup_wifi(){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      Serial.println(client.state());
      // Once connected, publish an announcement...
      client.publish(topic_setting,"hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

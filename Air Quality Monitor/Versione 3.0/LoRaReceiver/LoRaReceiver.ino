#include <LoRa.h> // Libreria modulo Lora
//#include <SPI.h>

#include <WiFi.h> // Libreria interfaccia WiFi
#include "EspMQTTClient.h" // Libreria per la gestione delle connessioni WiFi ed MQTT. Dipende dalla libreria ufficiale PubSubClient.h

// Definizione Pin per interfacciare il modulo Lora alla scheda ESP32
#define ss 5
#define rst 14
#define dio0 2

// Dichiarazione degli indirizzi dei nodi e del gateway utilizzati nelle connessioni Lora
byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;

String LoRaData = ""; // Stringa per immagazzinare il messaggio ricevuto dagli altri moduli

EspMQTTClient client(
  "********",   // Nome SSID Wifi
  "********",             // Password Wifi
  "***************",    // MQTT Broker server ip
  "ESP32",                // Nome client che identifica il device
  1883                    // Porta MQTT
);

// Topic per comunicaione con Broker MQTT
const char *topic_node1 = "esp32/qualutyaircheck99_node1";
const char *topic_node2 = "esp32/qualutyaircheck99_node2";
const char *topic_setting = "esp32/qualutyaircheck99_setting";


void setup() 
{
  Serial.begin(115200);                         // Apertura comunicazione seriale
  while (!Serial);
  Serial.println("LoRa Receiver");
 
  LoRa.setPins(ss, rst, dio0);                  // Setup modulo LoRa
 
  while (!LoRa.begin(433E6))                    // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  
  LoRa.setSyncWord(0xA5);                       // Setup SyncWord per evitare connessioni da tutti i nodi che trasmettono con il protocollo Lora
  Serial.println("LoRa Initializing OK!");

}

void loop() {
  client.loop();                                // Funzione richiamata per verificare la presenza di nuovi messaggi
  onReceive(LoRa.parsePacket());                // Funzione per accettare i messaggi in ingresso ed effettuare publish sul Broker MQTT
}

void onConnectionEstablished()                  // Funzione richiamata dalla libreria EspMQTTClient.h quando il device è connesso sia ad Internet che al Broker MQTT
{

    client.publish(topic_setting, "Connesso!!!");

}



void onReceive(int packetSize) {
  if (packetSize == 0) return;          // Se non ci sono pacchetti in ingresso esce

  // Lettura byte in ingresso
  int recipient = LoRa.read();          // Byte che identifica il destinatario
  byte sender = LoRa.read();            // Byte che identifica il mittente
  byte incomingLength = LoRa.read();    // Byte che identifica la lunghezza del messaggio
  
    while (LoRa.available()) {          // Lettura stringa formattata secondo lo standard JSON
    LoRaData += (char)LoRa.read();
  }
  
  if (incomingLength != LoRaData.length()) {        // Controllo se la lunghezza del messaggio ricevuto è uguale alla lunghezza ricevuta nel byte di controllo,
    return;                                         // se negativo esce, poichè c'è stato qualche errore nella trasmissione
  }
  
  
  if (recipient != MasterNode) {                    // Controllo che il byte che identifica il nodo master sia uguale all'identificativo conosciuto,
    return;                                         // se negativo esce, poichè il destinatario del pacchetto è un altro nodo                      
  }

  // Conversione del messaggio da stringa ad array di char, poichè la funzione publish della libreria PubSubClient.h accetta array di char come parametro
  int str_len = LoRaData.length() + 1;
  char char_array[str_len];
  LoRaData.toCharArray(char_array, str_len);

  // Si effettua il confronto del byte identificativo del mittente per verificare su quale topic mqtt effettuare la publish
  if ( sender == 0XBB )
  {
    client.publish(topic_node1,char_array);
  }
  
  if ( sender == 0XCC )
  {
    client.publish(topic_node2,char_array);
  }
  
  LoRaData = "";  // Svuotamento stringa contenente il messaggio

}

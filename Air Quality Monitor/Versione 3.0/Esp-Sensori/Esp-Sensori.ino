#include <ArduinoJson.h>  // Libreria per la gestione di stringhe in formato JSON

#include "DHT.h"  // Libreria per la gestione del sensore DHT11 (temperatura ed umidità)
#include <MQUnifiedsensor.h>  // Libreria per la gestione del sensore MQ-135 (CO, CO2, ecc.)

#include <LoRa.h> // Libreria per la gestione del modulo Lora
//#include <SPI.h>

// Definizione Pin per interfacciare il modulo Lora alla scheda ESP32
#define ss 5
#define rst 14
#define dio0 2

// Definizione Pin per interfacciare il sensore DHT11 con la scheda ESP32. Definizione del tipo di sensore utilizzato
#define DHTPIN 4
#define DHTTYPE DHT11

// Definizione impostazioni di setup del sensore MQ-135
#define device "ESP-32"
#define Voltage_Resolution 5
#define pin 33 
#define type "MQ-135" 
#define ADC_Bit_Resolution 12 
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  
//#define calibration_button 13 //Pin to calibrate your sensor

// Funzione per dichiarare il sensore MQ-135
MQUnifiedsensor MQ135(device, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

// Dichiarazione sensore DHT11
DHT dht(DHTPIN, DHTTYPE);

// Dichiarazione variabili necessarie per immagazzinare i valori letti dai vari sensori
float h = 0;
float t = 0;
float hic = 0;
float CO = 0;
float Alcohol = 0;
float CO2 = 0;
float NH4 = 0;
float Aceton = 0;


//String sendstring = "";

// Dichiarazione degli indirizzi del nodo master e del nodo mittente
byte MasterNode = 0xFF;
byte Node = 0xBB;

void setup() {
  Serial.begin(9600); // Apertura comunicazione seriale
  
  dht.begin();        // Inizializzazione sensore DHT11
  
  MQ135.setRegressionMethod(1); // Definizione del metodo utilizzato per il calcolo dei vari valori letti dal sensore MQ-135 _PPM =  a*ratio^b
  MQ135.init();                 // Inizializzazione sensore MQ-135

  /**********************************MQ CAlibration***************************************/
  // Codice utilizzato per calibrare il sensore MQ-135. Il codice proviene da uno sketch di base presente come esempio nella libreria
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 

LoRa.setPins(ss, rst, dio0);      // Setup modulo LoRa
  
  while (!LoRa.begin(433E6))      // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);         // Setup SyncWord per evitare connessioni da tutti i nodi che trasmettono con il protocollo Lora
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  StaticJsonDocument<200> doc;                  // Dichiarazione oggetto JSON
  lettura_temp_hum(h,t,hic);                    // Funzione per leggere i dati dal sensore DHT11
  lettura_mq135(CO,Alcohol,CO2,NH4,Aceton);     // Funzione per leggere i dati dal sensore MQ-135
  
  /***********************Riempimento oggetto JSON*****************************/
  doc["humidity"] = h;
  doc["temperature"] = t;
  doc["hic"] = hic;
  doc["CO"] = CO;
  doc["Alcohol"] = Alcohol;
  doc["CO2"] = CO2+400;
  doc["NH4"] = NH4;
  doc["Aceton"] = Aceton;
  /************************Riempimento oggetto JSON*****************************/

  String sensor = doc.as<String>();             // Conversione oggetto JSON in stringa
  Serial.println(sensor);
  //serializeJson(doc,Serial);

  LoRa.beginPacket();                           // Avvio pacchetto Lora
  LoRa.write(MasterNode);                       // Invio byte che identifica il destinatario
  LoRa.write(Node);                             // Invio byte che identifica il mittente
  LoRa.write(sensor.length());                  // Invio lunghezza del messaggio
  LoRa.print(sensor);                           // Invio messaggio contenente i valori letti dai sensori
  LoRa.endPacket();                             // Chiusura trasmissione pacchetto

  delay(6000);                                  // Delay tecnico poichè i sensori non possono effettuare tante letture velocemente
}

// Funzione per la lettura dei dati dal sensore DHT11
void lettura_temp_hum(float &h, float &t, float &hic) {
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);

}

// Funzione per la lettura dei dati dal sensore MQ-135. Da notare che il sensore in questione
// legge un solo valore tramite il variare di una resistenza interna, il quale viene passato ad un modello
// matematico che permette il calcolo delle diverse concentrazioni delle varie componenti. 
void lettura_mq135(float &CO, float &Alcohol, float &CO2, float &NH4, float &Aceton) {
  MQ135.update();

  MQ135.setA(605.18); MQ135.setB(-3.937);
  CO = MQ135.readSensor();

  MQ135.setA(77.255); MQ135.setB(-3.18);
  Alcohol = MQ135.readSensor();

  MQ135.setA(110.47); MQ135.setB(-2.862);
  CO2 = MQ135.readSensor();

  //MQ135.setA(44.947); MQ135.setB(-3.445);
  //Toluen = MQ135.readSensor();

  MQ135.setA(102.2 ); MQ135.setB(-2.473);
  NH4 = MQ135.readSensor();

  MQ135.setA(34.668); MQ135.setB(-3.369);
  Aceton = MQ135.readSensor();
}

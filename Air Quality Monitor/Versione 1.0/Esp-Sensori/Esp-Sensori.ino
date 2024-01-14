#include "DHT.h"
#include <MQUnifiedsensor.h>
#include <LoRa.h>
#include <SPI.h>


#define ss 5
#define rst 14
#define dio0 2

#define DHTPIN 4
#define DHTTYPE DHT11

#define placa "ESP-32"
#define Voltage_Resolution 5
#define pin 33 //Analog input 0 of your arduino
#define type "MQ-135" //MQ135
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6//RS / R0 = 3.6 ppm  
//#define calibration_button 13 //Pin to calibrate your sensor


MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

DHT dht(DHTPIN, DHTTYPE);

float h = 0;
float t = 0;
float hic = 0;
float CO = 0;
float Alcohol = 0;
float CO2 = 0;
float Toluen = 0;
float NH4 = 0;
float Aceton = 0;

String sendstring = "";

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.init(); 

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
  lettura_temp_hum(h,t,hic);
  lettura_mq135(CO,Alcohol,CO2,Toluen,NH4,Aceton);
  String sensor = String(h)+"-"+String(t)+"-"+String(hic)+"-"+String(CO)+"-"+String(Alcohol)+"-"+String(CO2+400)+"-"+String(Toluen)+"-"+String(NH4)+"-"+String(Aceton);
  Serial.println(sensor);
  LoRa.beginPacket();
  LoRa.println(sensor);
  LoRa.endPacket();
  delay(5000);
}

void lettura_temp_hum(float &h, float &t, float &hic) {
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);

}


void lettura_mq135(float &CO, float &Alcohol, float &CO2, float &Toluen, float &NH4, float &Aceton) {
  MQ135.update();

  MQ135.setA(605.18); MQ135.setB(-3.937);
  CO = MQ135.readSensor();

  MQ135.setA(77.255); MQ135.setB(-3.18);
  Alcohol = MQ135.readSensor();

  MQ135.setA(110.47); MQ135.setB(-2.862);
  CO2 = MQ135.readSensor();

  MQ135.setA(44.947); MQ135.setB(-3.445);
  Toluen = MQ135.readSensor();

  MQ135.setA(102.2 ); MQ135.setB(-2.473);
  NH4 = MQ135.readSensor();

  MQ135.setA(34.668); MQ135.setB(-3.369);
  Aceton = MQ135.readSensor();
}

# QualityAir

Sistema per il controllo della qualità dell'aria in un'azienda. Diverse schede Esp32 sono configurate con sensori MQ-135 e DHT11 ed un modulo LoRa.
Ogni scheda Esp legge i dati e comunica con un gateway Lora (Altra scheda Esp dotata di modulo LoRa), il quale riceve i dati ed effettua publish MQTT
ad un broker. Infine con Node-Red è possibile visualizzare i dati in tempo reale attraverso una dashboard.

## Materiale necessario

Per la realizzazione del nodo Gateway LoRa:
- ESP32 WiFi
- Modulo LoRa sx1278

Per la realizzazione di ogni nodo per le rilevazioni:
- ESP32 WiFi
- Modulo LoRa sx1278
- DHT11 Sensor
- MQ135 Sensor

## Collegamenti hardware
Su entrambe le schede ESP32 collegare il modulo LoRa come nello schema seguente:

![ESP32-LoRa-SX1278-1](https://github.com/giovannirinaldi6/QualityAir/assets/34186044/2c427ba3-4a83-45de-a3d5-3df979a85910)

### Collegamenti hardware solo nodi equipaggiati con sensori
Collegare il sensore DHT11 al pin 4. Il sensore MQ-135 al pin 33.


## Librerie necessarie

- LoRa.h
- WiFi.h
- PubSubClient.h
- EspMQTTClient.h
- ArduinoJson.h
- DHT.h
- MQUnifiedsensor.h

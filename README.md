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

## Librerie necessarie

- LoRa.h
- WiFi.h
- PubSubClient.h
- EspMQTTClient.h
- ArduinoJson.h
- DHT.h
- MQUnifiedsensor.h

Versione 3.0
Nei nodi dotati di sensori sono stati aggiunti byte nella trasmissione LoRa che identificano rispettivamente:
- Id mittente
- Id destinatario
- Lunghezza messaggio

Il nodo gateway ora è in grado di ricevere da più nodi ed è in grado di smistare sul topic MQTT corretto in relazione
al nodo mittente.

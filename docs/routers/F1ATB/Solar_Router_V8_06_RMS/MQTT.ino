// **********************************************************************************************
// *                        MQTT AUTO-DISCOVERY POUR HOME ASSISTANT ou DOMOTICZ                            *
// **********************************************************************************************
bool Discovered = false;


// Types de composants reconnus par HA et obligatoires pour l'Auto-Discovery.
const char *SSR = "sensor";
const char *SLCT = "select";
const char *NB = "number";
const char *BINS = "binary_sensor";
const char *SWTC = "switch";
const char *TXT = "text";

void envoiAuMQTT() {  // Cette Fonction d'origine a été modifiée
  unsigned long tps = millis();
  int etat = 0;                                                                // utilisé pour l'envoie de l'état On/Off des actions.
  if (int((tps - previousMqttMillis) / 1000) > MQTTRepet && MQTTRepet != 0) {  // Si Service MQTT activé avec période sup à 0
    previousMqttMillis = tps;
    if (!clientMQTT.connected()) {  // si le mqtt n'est pas connecté (utile aussi lors de la 1ere connexion)
      Serial.println("Connection au serveur MQTT ...");
      byte arr[4];
      arr[0] = MQTTIP & 0xFF;          // 0x78
      arr[1] = (MQTTIP >> 8) & 0xFF;   // 0x56
      arr[2] = (MQTTIP >> 16) & 0xFF;  // 0x34
      arr[3] = (MQTTIP >> 24) & 0xFF;  // 0x12
      String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
      clientMQTT.setServer(host.c_str(), MQTTPort);
      clientMQTT.setCallback(callback);                                                     //Déclaration de la fonction de souscription
      if (clientMQTT.connect(MQTTdeviceName.c_str(), MQTTUser.c_str(), MQTTPwd.c_str())) {  // si l'utilisateur est connecté au mqtt
        Serial.println(MQTTdeviceName + "connecté");
        Debug.println(MQTTdeviceName + "connecté");

      } else {  // si utilisateur pas connecté au mqtt
        Serial.print("echec connexion au MQTT, code erreur= ");
        Serial.println(clientMQTT.state());
        Debug.print("echec connexion au MQTT, code erreur= ");
        Debug.println(clientMQTT.state());
        StockMessage("Echec connexion MQTT : " + host);
        return;
      }
    }  // ici l'utilisateur est normalement connecté au mqtt
    clientMQTT.loop();

    if (!Discovered) {  //(uniquement au démarrage discovery = 0)
      sendMQTTDiscoveryMsg_global();
    }
    SendDataToHomeAssistant();  // envoie du Payload au State topic
    clientMQTT.loop();
  }
}
//Callback  pour souscrire a un topic et  prévoir une action
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.println("-------Nouveau message du broker mqtt. Non utilisé-----");
  Debug.println("-------Nouveau message du broker mqtt. Non utilisé-----");
}
//*************************************************************************
//*          CONFIG OF DISCOVERY MESSAGE FOR HOME ASSISTANT  / DOMOTICZ             *
//*************************************************************************


void sendMQTTDiscoveryMsg_global() {
  String ActType;
  // augmente la taille du buffer wifi Mqtt (voir PubSubClient.h)
  clientMQTT.setBufferSize(700);  // voir -->#define MQTT_MAX_PACKET_SIZE 256 is the default value in PubSubClient.h
  if (Source == "UxIx2" || Source == "ShellyEm") {
    DeviceToDiscover("PuissanceS_T", "W", "power", "0");
    DeviceToDiscover("PuissanceI_T", "W", "power", "0");
    DeviceToDiscover("Tension_T", "V", "voltage", "2");
    DeviceToDiscover("Intensite_T", "A", "current", "2");
    DeviceToDiscover("PowerFactor_T", "", "power_factor", "2");
    DeviceToDiscover("Energie_T_Soutiree", "Wh", "energy", "0");
    DeviceToDiscover("Energie_T_Injectee", "Wh", "energy", "0");
    DeviceToDiscover("EnergieJour_T_Soutiree", "Wh", "energy", "0");
    DeviceToDiscover("EnergieJour_T_Injectee", "Wh", "energy", "0");
    DeviceToDiscover("Frequence", "Hz", "frequency", "2");
  }
  if (temperature >-100) {
    DeviceToDiscover("Temperature", "°C", "temperature", "1");
  }
  
  if (Source == "Linky"){
    DeviceTextToDiscover("LTARF", "Option Tarifaire");
    DeviceToDiscover("Code_Tarifaire", "", "", "0");
  }

  DeviceToDiscover("PuissanceS_M", "W", "power", "0");
  DeviceToDiscover("PuissanceI_M", "W", "power", "0");
  DeviceToDiscover("Tension_M", "V", "voltage", "2");
  DeviceToDiscover("Intensite_M", "A", "current", "2");
  DeviceToDiscover("PowerFactor_M", "", "power_factor", "2");
  DeviceToDiscover("Energie_M_Soutiree", "Wh", "energy", "0");
  DeviceToDiscover("Energie_M_Injectee", "Wh", "energy", "0");
  DeviceToDiscover("EnergieJour_M_Soutiree", "Wh", "energy", "0");
  DeviceToDiscover("EnergieJour_M_Injectee", "Wh", "energy", "0");

  for (int i = 0; i < NbActions; i++) {
    ActType="Ouverture_Relais_" + String(i);
    if (i==0) ActType="OuvertureTriac" ;
    DeviceToDiscover(ActType, "%", "power_factor", "0");  //Type power factor pour etre accepté par HA
  }


  Serial.println("Paramètres Auto-Discovery publiés !");
  Debug.println("Paramètres Auto-Discovery publiés !");

  //clientMQTT.setBufferSize(512);  // go to initial value wifi/mqtt buffer
  Discovered = true;


}  // END OF sendMQTTDiscoveryMsg_global

void DeviceToDiscover(String Name, String Unit, String Class, String Round) {

  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  DynamicJsonDocument doc(512);  // this is the Payload json format
  JsonObject device;             // for device object  "device": {}
  JsonArray option;              // options (array) of this device
  char buffer[512];
  size_t n;
  bool published;

  String ESP_ID = WiFi.macAddress();  //The chip ID is essentially its MAC address.                                        // ID de l'entité pour HA
  String DiscoveryTopic;              // nom du topic pour ce capteur

  //DiscoveryTopic = ConfigTopic(Name, SSR, "config");
  DiscoveryTopic = String(MQTTPrefix) + "/" + String(SSR) + "/" + String(MQTTdeviceName) + "_" + String(Name) + "/" + String("config");
  doc["name"] = String(MQTTdeviceName) + " " + String(Name);     // concatenation des ID uniques des entités (capteurs ou commandes)
  doc["uniq_id"] = String(MQTTdeviceName) + "_" + String(Name);  // concatenation des Noms uniques des entités
  doc["stat_t"] = StateTopic;
  doc["unit_of_meas"] = Unit;
  if (Unit=="Wh"){
      doc["state_class"] = "total_increasing";
  }
  doc["device_class"] = Class;
  doc["val_tpl"] = "{{ value_json." + Name + "|default(0)| round(" + Round + ") }}";
  device = doc.createNestedObject("device");
  device["ids"][0] = ESP_ID;  // identification unique sur Home Assistant (obligatoire).
  device["cu"] = "http://" + WiFi.localIP().toString();
  device["hw"] = String(ESP.getChipModel()) + " rev." + String(ESP.getChipRevision());
  device["mf"] = "F1ATB - https://f1atb.fr";
  device["mdl"] = "ESP32 - " + ESP_ID;
  device["name"] = nomRouteur;
  device["sw"] = Version;

  n = serializeJson(doc, buffer);
  published = clientMQTT.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}
void DeviceBinToDiscover(String Name, String Titre) {

  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  DynamicJsonDocument doc(512);  // this is the Payload json format
  JsonObject device;             // for device object  "device": {}
  JsonArray option;              // options (array) of this device
  char buffer[512];
  size_t n;
  bool published;

  String ESP_ID = WiFi.macAddress();  //The chip ID is essentially its MAC address.                                        // ID de l'entité pour HA
  String DiscoveryTopic;              // nom du topic pour ce capteur

 
  DiscoveryTopic = String(MQTTPrefix) + "/" + String(BINS) + "/" + String(MQTTdeviceName) + "_" + String(Name) + "/" + String("config");
  doc["name"] = String(MQTTdeviceName) + " " + String(Titre);    // concatenation des ID uniques des entités (capteurs ou commandes)
  doc["uniq_id"] = String(MQTTdeviceName) + "_" + String(Name);  // concatenation des Noms uniques des entités
  doc["stat_t"] = StateTopic;
  doc["init"] = "OFF";  // default value
  doc["ic"] = "mdi:electric-switch";
  doc["val_tpl"] = "{{ value_json." + Name + " }}";
  device = doc.createNestedObject("device");
  device["ids"][0] = ESP_ID;
  device["name"] = nomRouteur;
  n = serializeJson(doc, buffer);
  published = clientMQTT.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}

void DeviceTextToDiscover(String Name, String Titre) {
  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  DynamicJsonDocument doc(512);  // this is the Payload json format
  JsonObject device;             // for device object  "device": {}
  JsonArray option;              // options (array) of this device
  char buffer[512];
  size_t n;
  bool published;
  String ESP_ID = WiFi.macAddress();  //The chip ID is essentially its MAC address.                                        // ID de l'entité pour HA
  String DiscoveryTopic;              // nom du topic pour ce capteur
  DiscoveryTopic = String(MQTTPrefix) + "/" + String(SSR) + "/" + String(MQTTdeviceName) + "_" + String(Name) + "/" + String("config");
  doc["name"] = String(MQTTdeviceName) + " " + String(Titre);    // concatenation des ID uniques des entités (capteurs ou commandes)
  doc["uniq_id"] = String(MQTTdeviceName) + "_" + String(Name);  // concatenation des Noms uniques des entités
  doc["stat_t"] = StateTopic;
  doc["device_class"] = "enum";
  doc["val_tpl"] = "{{ value_json." + Name + " }}";
  device = doc.createNestedObject("device");
  device["ids"][0] = ESP_ID;
  device["name"] = nomRouteur;
  n = serializeJson(doc, buffer);
  published = clientMQTT.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}
//****************************************
//* ENVOIE DES DATAS VERS HOME ASSISTANT *
//****************************************

void SendDataToHomeAssistant() {

  //common state topic for all entities of this ESP32 Device
  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  String ActType;
  DynamicJsonDocument doc(1024);  // 1024 octets = capacité  largement suffisante
  char buffer[1024];
  if (Source == "UxIx2"  || Source == "ShellyEm") {
    doc["PuissanceS_T"] = PuissanceS_T;  // Triac
    doc["PuissanceI_T"] = PuissanceI_T;  // Triac
    doc["Tension_T"] = Tension_T;
    doc["Intensite_T"] = Intensite_T;
    doc["PowerFactor_T"] = PowerFactor_T;
    doc["Energie_T_Soutiree"] = Energie_T_Soutiree;
    doc["Energie_T_Injectee"] = Energie_T_Injectee;
    doc["EnergieJour_T_Soutiree"] = EnergieJour_T_Soutiree;
    doc["EnergieJour_T_Injectee"] = EnergieJour_T_Injectee;
    doc["Frequence"] = Frequence;
  }
  if (temperature >-100) {
    doc["Temperature"] = temperature;
  }
  if (Source == "Linky"){
    
    doc["LTARF"]= LTARF;
    int code=0;
    if (LTARF.indexOf("HEURE  CREUSE")>=0) code=1; //Code Linky
    if (LTARF.indexOf("HEURE  PLEINE")>=0) code=2;
    if (LTARF.indexOf("HC BLEU")>=0) code=11;
    if (LTARF.indexOf("HP BLEU")>=0) code=12;
    if (LTARF.indexOf("HC BLANC")>=0) code=13;
    if (LTARF.indexOf("HP BLANC")>=0) code=14;
    if (LTARF.indexOf("HC ROUGE")>=0) code=15;
    if (LTARF.indexOf("HP ROUGE")>=0) code=16;
    if (LTARF.indexOf("TEMPO_BLEU")>=0) code=17; // Code EDF
    if (LTARF.indexOf("TEMPO_BLANC")>=0) code=18;
    if (LTARF.indexOf("TEMPO_ROUGE")>=0) code=19;
    doc["Code_Tarifaire"]= code;
  }
  doc["PuissanceS_M"] = PuissanceS_M ;  // Maison
  doc["PuissanceI_M"] = PuissanceI_M;  // Maison
  doc["Tension_M"] = Tension_M;
  doc["Intensite_M"] = Intensite_M;
  doc["PowerFactor_M"] = PowerFactor_M;
  doc["Energie_M_Soutiree"] = Energie_M_Soutiree;
  doc["Energie_M_Injectee"] = Energie_M_Injectee;
  doc["EnergieJour_M_Soutiree"] = EnergieJour_M_Soutiree;
  doc["EnergieJour_M_Injectee"] = EnergieJour_M_Injectee;

  for (int i = 0; i < NbActions; i++) {
    ActType="Ouverture_Relais_" + String(i);
    if (i==0) ActType="OuvertureTriac" ;
    doc[ActType] = 100 - Retard[i];
  }



  size_t n = serializeJson(doc, buffer);
  bool published = clientMQTT.publish(StateTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';



}  // END SendDataToHomeAssistant

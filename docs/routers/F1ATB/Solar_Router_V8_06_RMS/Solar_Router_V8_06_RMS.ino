/*
  PV Router / Routeur Solaire 
  ****************************************
  Version V_8.06_RMS 

  RMS=Routeur Multi Sources

  Choix de 5 sources différentes pour lire la consommation électrique en entrée de maison
  - lecture de la tension avec un transformateur et du courant avec une sonde ampèremétrique (UxI)
  - lecture des données du Linky (Linky)
  - module (JSY-MK-194T) intégrant une mesure de tension secteur et 2 sondes ampèmétriques (UxIx2)
  - Lecture passerelle Enphase - Envoy-S metered (firmware V5 et V7)
  - Lecture depuis un autre ESP qui comprend l'une des 4 sources citées plus haut
  - Lecture compteur SmartG (en test)
  
  En option une mesure de température (DS18B20) est possible.

  Historique des versions
  - V6.00_RMS Corrige un bug sur le nom du capteur de température et retire les mauvaises mesure de température
              Introduit la source Smart Gateways des compteurs belge, holladais etc. (en test)
              Introduit la source Shelly Em
              Introduit un code tarifaire numérique en plus de LTARF dans les messages MQTT si Linky
              Encode les URL vers Enphase pour passer les mots de passe
  - V6.01_RMS Recadre la courbe des températures sur 48h
              Affiche les valeurs des courbes suivant la postion du curseur en X
              Corrige un bug d'ocillation d'un relais externe pendant 1mn au début de l'action
  - V6.02_RMS Corrige un bug avec un Shelly en triphasé et en injection
  - V7.00_RMS Donne la couleur Tempo du jour et du lendemain donnée par EDF(pour ceux disposant de cette tarfication)
              Facilite la connexion au Wifi local lors de la première connexion
              Pour le Shelly, force Pva=0 quand Pw=0;
  - V7.01_RMS Corrige un bug sur l'affichage de l'énergie quotidienne dans le cas ESP externe
  - V7.02_RMS Prend en compe les caratères non alphabétiques dans le mot de passe réseau
  - V7.03_RMS Corrige un bug d'accès wifi vers Envoy et EDF
  - V8.00_RMS Crée les modes multi-sinus et train de sinus
              Change la bibliothèque pour la liaison série vers Linky ou UxIx2
  - V8.01_RMS Corrige un bug sur l'envoi MQTT du Triac
  - V8.02_RMS Modifie les libellés des relais On/Off dans la page d'accueil
  - V8.03_RMS Lissage voie secondaire pour le capteur Shelly
  - V8.04_RMS Rajoute un lissage sur les puissances uniquement si multi-sinus ou train de sinus avec une mesure de puissance courte
  - V8.05_RMS Rajoute un lissage sur les puissances voie secondaire Shelly 
  - V8.06_RMS Rajout device["name"] = nomRouteur; dans DeviceTextToDiscover
              Début couleur Tempo du jour à 6h00 du matin et non Oh
              Modification loi de lissage pour les mesures courtes. Pour les multi-sinus et train-sinus.
              Correction bug sur régulation On/Off
  
  
  Les détails sont disponibles sur / Details are available here:
  https://f1atb.fr  Section Domotique / Home Automation

  F1ATB Mars 2024 

  GNU General Public License v3.0



*/
#define Version "8.06_RMS"
#define HOSTNAME "RMS-ESP32-"
#define CLE_Rom_Init 812567808  //Valeur pour tester si ROM vierge ou pas. Un changement de valeur remet à zéro toutes les données. / Value to test whether blank ROM or not.


//Librairies
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <ArduinoOTA.h>    //Modification On The Air
#include <RemoteDebug.h>   //Debug via Wifi
#include <esp_task_wdt.h>  //Pour un Watchdog
#include <PubSubClient.h>  //Librairie pour la gestion Mqtt
#include "EEPROM.h"        //Librairie pour le stockage en EEPROM historique quotidien
#include "esp_sntp.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include <ArduinoJson.h>
#include "UrlEncode.h"
#include <HardwareSerial.h>

//Program routines
#include "pageHtmlBrute.h"
#include "pageHtmlMain.h"
#include "pageHtmlConnect.h"
#include "pageHtmlPara.h"
#include "pageHtmlActions.h"
#include "Actions.h"


//Watchdog de 180 secondes. Le systeme se Reset si pas de dialoque avec le LINKY ou JSY-MK-194T ou Enphase-Envoye pendant 180s
//Watchdog for 180 seconds. The system resets if no dialogue with the Linky or  JSY-MK-194T or Enphase-Envoye for 180s
#define WDT_TIMEOUT 180

//PINS - GPIO

const int AnalogIn0 = 35;  //Pour Routeur Uxi
const int AnalogIn1 = 32;
const int AnalogIn2 = 33;  //Note: si GPIO 33 non disponible sur la carte ESP32, utilisez la 34. If GPIO 33 not available on the board replace by GPIO 34
#define RXD2 26            //Pour Routeur Linky ou UxIx2 (sur carte ESP32 simple): Couple RXD2=26 et TXD2=27 . Pour carte ESP32 4 relais : Couple RXD2=17 et TXD2=27
#define TXD2 27
#define SER_BUF_SIZE 4096
#define LedYellow 18
#define LedGreen 19
#define pulseTriac 22
#define zeroCross 23
#define pinTemp 13  //Capteur température


//Nombre Actions Max
#define LesActionsLength 20
//VARIABLES
const char *ap_default_ssid;        // Mode Access point  IP: 192.168.4.1
const char *ap_default_psk = NULL;  // Pas de mot de passe en AP,

//Paramètres dans l'odre de stockage en ROM apres les données du RMS
unsigned long Cle_ROM;

String ssid = "";
String password = "";
String Source = "UxI";
String Source_data = "UxI";
byte dhcpOn = 1;
unsigned long IP_Fixe = 0;
unsigned long Gateway = 0;
unsigned long masque = 4294967040;
unsigned long dns = 0;
unsigned long RMSextIP = 0;
unsigned int MQTTRepet = 0;
unsigned long MQTTIP = 0;
unsigned int MQTTPort = 1883;
String MQTTUser = "User";
String MQTTPwd = "password";
String MQTTPrefix = "homeassistant";  // prefix obligatoire pour l'auto-discovery entre HA et Core-Mosquitto (par défaut c'est homeassistant)
String MQTTdeviceName = "routeur_rms";
String nomRouteur = "Routeur - RMS";
String nomSondeFixe = "Données seconde sonde";
String nomSondeMobile = "Données Maison";
String nomTemperature = "Température";
String GS = String((char)29);  //Group Separator
String RS = String((char)30);  //Record Separator
String Message[4];
int idxMessage = 0;
int P_cent_EEPROM;
int cptLEDyellow = 0;
int cptLEDgreen = 0;

unsigned int CalibU = 1000;  //Calibration Routeur UxI
unsigned int CalibI = 1000;
int value0;
int volt[100];
int amp[100];
float KV = 0.2083;  //Calibration coefficient for the voltage. Value for CalibU=1000 at startup
float KI = 0.0642;  //Calibration coefficient for the current. Value for CalibI=1000 at startup
float kV = 0.2083;  //Calibration coefficient for the voltage. Corrected value
float kI = 0.0642;  //Calibration coefficient for the current. Corrected value
float voltM[100];   //Voltage Mean value
float ampM[100];

bool EnergieActiveValide = false;
long EAS_T_J0 = 0;
long EAI_T_J0 = 0;
long EAS_M_J0 = 0;  //Debut du jour energie active
long EAI_M_J0 = 0;


int adr_debut_para = 0;  //Adresses Para après le Wifi


//Paramètres électriques
float Tension_T, Intensite_T, PowerFactor_T, Frequence;
float Tension_M, Intensite_M, PowerFactor_M;
long Energie_T_Soutiree = 0;
long Energie_T_Injectee = 0;
long Energie_M_Soutiree = 0;
long Energie_M_Injectee = 0;
long EnergieJour_T_Injectee = 0;
long EnergieJour_M_Injectee = 0;
long EnergieJour_T_Soutiree = 0;
long EnergieJour_M_Soutiree = 0;
int PuissanceS_T, PuissanceS_M, PuissanceI_T, PuissanceI_M;
int PVAS_T, PVAS_M, PVAI_T, PVAI_M;
float PuissanceS_T_inst, PuissanceS_M_inst, PuissanceI_T_inst, PuissanceI_M_inst;
float PVAS_T_inst, PVAS_M_inst, PVAI_T_inst, PVAI_M_inst;
float Puissance_T_moy, Puissance_M_moy;
float PVA_T_moy, PVA_M_moy;
int PactConso_M, PactProd;
int tabPw_Maison_5mn[600];  //Puissance Active:Soutiré-Injecté toutes les 5mn
int tabPw_Triac_5mn[600];
int tabTemperature_5mn[600];
int tabPw_Maison_2s[300];   //Puissance Active: toutes les 2s
int tabPw_Triac_2s[300];    //Puissance Triac: toutes les 2s
int tabPva_Maison_2s[300];  //Puissance Active: toutes les 2s
int tabPva_Triac_2s[300];
int tabPulseSinusOn[101];
int tabPulseSinusTotal[101];
int IdxStock2s = 0;
int IdxStockPW = 0;
float PmaxReseau = 36000;  //Puissance Max pour eviter des débordements
bool LissageLong = false;

//Parameters for JSY-MK-194T module
byte ByteArray[130];
long LesDatas[14];
int Sens_1, Sens_2;

//Parameters for Linky
bool LFon = false;
bool EASTvalid = false;
bool EAITvalid = false;
volatile int IdxDataRawLinky = 0;
volatile int IdxBufDecodLinky = 0;
volatile char DataRawLinky[10000];  //Buffer entrée données Linky
float moyPWS = 0;
float moyPWI = 0;
float moyPVAS = 0;
float moyPVAI = 0;
float COSphiS = 1;
float COSphiI = 1;
long TlastEASTvalide = 0;
long TlastEAITvalide = 0;
String LTARF = "";  //Option tarifaire EDF
String STGE = "";   //Status Tempo uniquement EDF

//Paramètres for Enphase-Envoye-Smetered
String TokenEnphase = "";
String EnphaseUser = "";
String EnphasePwd = "";
String EnphaseSerial = "0";  //Sert égalemnet au Shelly comme numéro de voie
String JsonToken = "";
String Session_id = "";
long LastwhDlvdCum = 0;  //Dernière valeur cumul Wh Soutire-injecté.
float EMI_Wh = 0;        //Energie entrée Maison Injecté Wh
float EMS_Wh = 0;        //Energie entrée Maison Injecté Wh

//Paramètres for SmartGateways
String SG_dataBrute = "";

//Paramètres for Shelly Em
String ShEm_dataBrute = "";
int ShEm_comptage_appels = 0;
float PwMoy2 = 0;  //Moyenne voie secondsaire
float pfMoy2 = 1;  //pf Voie secondaire

//Paramètres pour EDF
String DateEDF = "";  //an-mois-jour
byte TempoEDFon = 0;
int LastHeureEDF = -1;
int LTARFbin = 0;  //Code binaire  des tarifs



//Actions
Action LesActions[LesActionsLength];  //Liste des actions
volatile int NbActions = 0;



//Internal Timers
unsigned long startMillis;
unsigned long previousWifiMillis;
unsigned long previousHistoryMillis;
unsigned long previousWsMillis;
unsigned long previousWiMillis;
unsigned long LastRMS_Millis;
unsigned long previousTimer2sMillis;
unsigned long previousOverProdMillis;
unsigned long previousLEDsMillis;
unsigned long previousLoop;
unsigned long previousETX;
unsigned long PeriodeProgMillis = 1000;
unsigned long T0_seconde = 0;
unsigned long T_On_seconde = 0;
float previousLoopMin = 1000;
float previousLoopMax = 0;
float previousLoopMoy = 0;
unsigned long previousTimeRMS;
float previousTimeRMSMin = 1000;
float previousTimeRMSMax = 0;
float previousTimeRMSMoy = 0;
unsigned long previousMqttMillis;

//Actions et Triac(action 0)
float RetardF[LesActionsLength];  //Floating value of retard
//Variables in RAM for interruptions
volatile unsigned long lastIT = 0;
volatile int IT10ms = 0;     //Interruption avant deglitch
volatile int IT10ms_in = 0;  //Interruption apres deglitch
volatile int ITmode = 0;     //IT exerne Triac ou interne
hw_timer_t *timer = NULL;
hw_timer_t *timer10ms = NULL;


volatile int Retard[LesActionsLength];
volatile int Actif[LesActionsLength];
volatile int PulseOn[LesActionsLength];
volatile int PulseTotal[LesActionsLength];
volatile int PulseComptage[LesActionsLength];
volatile int Gpio[LesActionsLength];
volatile int OutOn[LesActionsLength];
volatile int OutOff[LesActionsLength];

WebServer server(80);  // Simple Web Server on port 80

//Port Serie 2 - Remplace Serial2 qui bug
HardwareSerial MySerial(2);

// Heure et Date
#define MAX_SIZE_T 80
const char *ntpServer1 = "fr.pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
String DATE = "";
String DateCeJour = "";
bool DATEvalid = false;
int HeureCouranteDeci = 0;
int idxPromDuJour = 0;

//Température Capteur DS18B20
OneWire oneWire(pinTemp);
DallasTemperature ds18b20(&oneWire);
float temperature = -127;  // La valeur vaut -127 quand la sonde DS18B20 n'est pas présente

//Debug via WIFI instead of Serial
//Connect a Telnet terminal on port 23
RemoteDebug Debug;
WiFiClient MqttClient;
PubSubClient clientMQTT(MqttClient);
int WIFIbug = 0;
WiFiClientSecure clientSecu;
WiFiClientSecure clientSecuEDF;

//Multicoeur - Processeur 0 - Collecte données RMS local ou distant
TaskHandle_t Task1;

//Interruptions, Current Zero Crossing from Triac device and Internal Timer
//*************************************************************************
void IRAM_ATTR onTimer10ms() {  //Interruption interne toutes 10ms
  ITmode--;
  if (ITmode < -5) ITmode = -5;
  if (ITmode < 0) GestionIT_10ms();  //IT non synchrone avec le secteur . Horloge interne
}


// Interruption du Triac Signal Zc, toutes les 10ms
void IRAM_ATTR currentNull() {
  IT10ms += 1;
  if ((millis() - lastIT) > 2) {  // to avoid glitch detection during 2ms
    ITmode++;
    ITmode++;
    if (ITmode > 5) ITmode = 5;
    IT10ms_in++;
    lastIT = millis();
    if (ITmode > 0) GestionIT_10ms();  //IT synchrone avec le secteur signal Zc
  }
}


void GestionIT_10ms() {
  for (int i = 0; i < NbActions; i++) {
    switch (Actif[i]) {  //valeur en RAM
      case 0:            //Inactif

        break;
      case 1:  //Decoupe Sinus uniquement pour Triac
        if (i == 0) {
          PulseComptage[0] = 0;
          digitalWrite(pulseTriac, LOW);  //Stop Découpe Triac
        }
        break;
      default:              // Multi Sinus ou Train de sinus
        if (Gpio[i] > 0) {  //Gpio valide
          if (PulseComptage[i] < PulseOn[i]) {
            digitalWrite(Gpio[i], OutOn[i]);
          } else {
            digitalWrite(Gpio[i], OutOff[i]);  //Stop
          }
          PulseComptage[i]++;
          if (PulseComptage[i] >= PulseTotal[i]) {
            PulseComptage[i] = 0;
          }
        }
        break;
    }
  }
}

// Interruption Timer interne toutes les 100 micro secondes
void IRAM_ATTR onTimer() {  //Interruption every 100 micro second
  if (Actif[0] == 1) {      // Découpe Sinus
    PulseComptage[0] += 1;
    if (PulseComptage[0] > Retard[0] && Retard[0] < 98 && ITmode > 0) {  //100 steps in 10 ms
      digitalWrite(pulseTriac, HIGH);                                    //Activate Triac
    } else {
      digitalWrite(pulseTriac, LOW);  //Stop Triac
    }
  }
}

// SETUP
//*******
void setup() {
  startMillis = millis();
  previousLEDsMillis = startMillis;

  //Pin initialisation
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(zeroCross, INPUT_PULLDOWN);
  pinMode(pulseTriac, OUTPUT);
  digitalWrite(LedYellow, LOW);
  digitalWrite(LedGreen, LOW);
  digitalWrite(pulseTriac, LOW);  //Stop Triac

  //Watchdog initialisation
  esp_task_wdt_init(WDT_TIMEOUT, true);  //enable panic so ESP32 restarts

  //Ports Série ESP
  Serial.begin(115200);
  Serial.println("Booting");

  for (int i = 0; i < LesActionsLength; i++) {
    LesActions[i] = Action(i);  //Creation objets
    PulseOn[i] = 0;             //1/2 sinus
    PulseTotal[i] = 100;
    PulseComptage[i] = 0;
    Retard[i] = 100;
    RetardF[i] = 100;
    OutOn[i] = 1;
    OutOff[i] = 0;
    Gpio[i] = -1;
  }
  Gpio[0] = pulseTriac;
  LesActions[0].Gpio = pulseTriac;

  //Tableau Longueur Pulse et Longueur Trame pour Multi-Sinus de 0 à 100%
  float erreur;
  float vrai;
  float target;
  for (int I = 0; I < 101; I++) {
    tabPulseSinusTotal[I] = -1;
    tabPulseSinusOn[I] = -1;
    target = float(I) / 100.0;
    for (int T = 20; T < 101; T++) {
      for (int N = 0; N <= T; N++) {
        if (T % 2 == 1 || N % 2 == 0) {  // Valeurs impair du total ou pulses pairs pour éviter courant continu
          vrai = float(N) / float(T);
          erreur = abs(vrai - target);
          if (erreur < 0.004) {
            tabPulseSinusTotal[I] = T;
            tabPulseSinusOn[I] = N;
            N = 101;
            T = 101;
          }
        }
      }
    }
  }

  init_puissance();
  esp_task_wdt_reset();

  // Configure WIFI
  // **************
  String hostname(HOSTNAME);
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  hostname += String(chipId);  //Add chip ID to hostname
  WiFi.hostname(hostname);
  Serial.println(hostname);
  ap_default_ssid = (const char *)hostname.c_str();
  // Check WiFi connection
  // ... check mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  INIT_EEPROM();
  //Lecture Clé pour identifier si la ROM a déjà été initialisée
  Cle_ROM = CLE_Rom_Init;
  unsigned long Rcle = LectureCle();
  Serial.println("cle : " + String(Rcle));
  if (Rcle == Cle_ROM) {  // Programme déjà executé
    LectureEnROM();
    LectureConsoMatinJour();
    InitGpioActions();
  } else {
    RAZ_Histo_Conso();
  }

  //Heure / Hour . A Mettre en priorité avant WIFI (exemple ESP32 Simple Time)
  //External timer to obtain the Hour and reset Watt Hour every day at 0h
  sntp_set_time_sync_notification_cb(time_sync_notification);
  sntp_servermode_dhcp(1);                                                               //Option
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer1, ntpServer2);  //Voir Time-Zone: https://sites.google.com/a/usapiens.com/opnode/time-zones



  //WIFI
  Serial.println("SSID:" + ssid);
  Serial.println("Pass:" + password);
  if (ssid.length() > 0) {
    if (dhcpOn == 0) {  //Static IP
      byte arr[4];
      arr[0] = IP_Fixe & 0xFF;          // 0x78
      arr[1] = (IP_Fixe >> 8) & 0xFF;   // 0x56
      arr[2] = (IP_Fixe >> 16) & 0xFF;  // 0x34
      arr[3] = (IP_Fixe >> 24) & 0xFF;  // 0x12
      // Set your Static IP address
      IPAddress local_IP(arr[3], arr[2], arr[1], arr[0]);
      // Set your Gateway IP address
      arr[0] = Gateway & 0xFF;          // 0x78
      arr[1] = (Gateway >> 8) & 0xFF;   // 0x56
      arr[2] = (Gateway >> 16) & 0xFF;  // 0x34
      arr[3] = (Gateway >> 24) & 0xFF;  // 0x12
      IPAddress gateway(arr[3], arr[2], arr[1], arr[0]);
      // Set your masque/subnet IP address
      arr[0] = masque & 0xFF;
      arr[1] = (masque >> 8) & 0xFF;
      arr[2] = (masque >> 16) & 0xFF;
      arr[3] = (masque >> 24) & 0xFF;
      IPAddress subnet(arr[3], arr[2], arr[1], arr[0]);
      // Set your DNS IP address
      arr[0] = dns & 0xFF;
      arr[1] = (dns >> 8) & 0xFF;
      arr[2] = (dns >> 16) & 0xFF;
      arr[3] = (dns >> 24) & 0xFF;
      IPAddress primaryDNS(arr[3], arr[2], arr[1], arr[0]);  //optional
      IPAddress secondaryDNS(8, 8, 4, 4);                    //optional
      if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("WIFI STA Failed to configure");
      }
    }
    StockMessage("Wifi Begin : " + ssid);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED && (millis() - startMillis < 15000)) {  // Attente connexion au Wifi
      Serial.write('.');
      Gestion_LEDs();
      Serial.print(WiFi.status());
      delay(300);
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    // ... print IP Address
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Connected IP address: " + WiFi.localIP().toString() + " or <a href='http://" + hostname + ".local' >" + hostname + ".local</a>");
    StockMessage("Connected IP address: " + WiFi.localIP().toString());
  } else {
    StockMessage("Can not connect to WiFi station. Go into AP mode and STA mode.");
    // Go into software AP and STA modes.
    WiFi.mode(WIFI_AP_STA);
    delay(10);
    WiFi.softAP(ap_default_ssid, ap_default_psk);
    Serial.print("Access Point Mode. IP address: ");
    Serial.println(WiFi.softAPIP());
  }

  // init remote debug
  Debug.begin("ESP32");
  Debug.println("Ready");
  Debug.print("IP address: ");
  Debug.println(WiFi.localIP());

  Init_Server();


  // Modification du programme par le Wifi  - OTA(On The Air)
  //***************************************************
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();  //Mandatory

  //Adaptation à la Source
  Serial.println("Source : " + Source);

  if (Source == "UxIx2") {
    Setup_UxIx2();
  }
  if (Source == "UxI") {
    Setup_UxI();
  }
  if (Source == "Linky") {
    Setup_Linky();
  }
  if (Source == "Enphase") {
    Setup_Enphase();
  }
  if (Source == "Ext") {
  } else {
    Source_data = Source;
  }

  xTaskCreatePinnedToCore(  //Préparation Tâche Multi Coeur
    Task_LectureRMS,        /* Task function. */
    "Task_LectureRMS",      /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    10,                     /* priority of the task */
    &Task1,                 /* Task handle to keep track of created task */
    0);                     /* pin task to core 0 */


  //Temperature
  ds18b20.begin();
  LectureTemperature();

  //Interruptions du Triac et Timer interne
  attachInterrupt(zeroCross, currentNull, RISING);

  //Hardware timer 100uS
  timer = timerBegin(0, 80, true);  //Clock Divider, 1 micro second Tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true);  //Interrupt every 100 Ticks or microsecond
  timerAlarmEnable(timer);

  //Hardware timer 10ms
  timer10ms = timerBegin(1, 80, true);  //Clock Divider, 1 micro second Tick
  timerAttachInterrupt(timer10ms, &onTimer10ms, true);
  timerAlarmWrite(timer10ms, 10000, true);  //Interrupt every 10000 Ticks or microsecond
  timerAlarmEnable(timer10ms);

  //Timers
  previousWifiMillis = millis() - 25000;
  previousHistoryMillis = millis() - 290000;
  previousTimer2sMillis = millis();
  previousLoop = millis();
  previousTimeRMS = millis();
  previousMqttMillis = millis() - 5000;
  previousETX = millis();
  previousOverProdMillis = millis();
  LastRMS_Millis = millis();
}

/* **********************
   * ****************** *
   * * Tâches Coeur 0 * *
   * ****************** *
   **********************
*/

void Task_LectureRMS(void *pvParameters) {
  esp_task_wdt_add(NULL);  //add current thread to WDT watch
  esp_task_wdt_reset();
  for (;;) {
    unsigned long tps = millis();
    float deltaT = float(tps - previousTimeRMS);
    previousTimeRMS = tps;
    previousTimeRMSMin = min(previousTimeRMSMin, deltaT);
    previousTimeRMSMin = previousTimeRMSMin + 0.001;
    previousTimeRMSMax = max(previousTimeRMSMax, deltaT);
    previousTimeRMSMax = previousTimeRMSMax * 0.9999;
    previousTimeRMSMoy = deltaT * 0.001 + previousTimeRMSMoy * 0.999;


    //Recupération des données RMS
    //******************************
    if (tps - LastRMS_Millis > PeriodeProgMillis) {  //Attention delicat pour eviter pb overflow
      LastRMS_Millis = tps;
      unsigned long ralenti = long(PuissanceS_M / 10);  // On peut ralentir échange sur Wifi si grosse puissance en cours
      if (Source == "UxI") {
        LectureUxI();
        PeriodeProgMillis = 40;
      }
      if (Source == "UxIx2") {
        LectureUxIx2();
        PeriodeProgMillis = 400;
      }
      if (Source == "Linky") {
        LectureLinky();
        PeriodeProgMillis = 2;
      }
      if (Source == "Enphase") {
        LectureEnphase();
        LastRMS_Millis = millis();
        PeriodeProgMillis = 200 + ralenti;  //On s'adapte à la vitesse réponse Envoye-S metered
      }
      if (Source == "SmartG") {
        LectureSmartG();
        LastRMS_Millis = millis();
        PeriodeProgMillis = 200 + ralenti;  //On s'adapte à la vitesse réponse SmartGateways
      }
      if (Source == "ShellyEm") {
        LectureShellyEm();
        LastRMS_Millis = millis();
        PeriodeProgMillis = 200 + ralenti;  //On s'adapte à la vitesse réponse ShellyEm
      }
      if (Source == "Ext") {
        CallESP32_Externe();
        LastRMS_Millis = millis();
        PeriodeProgMillis = 200 + ralenti;  //Après pour ne pas surchargé Wifi
      }
    }
    delay(2);
  }
}




/* **********************
   * ****************** *
   * * Tâches Coeur 1 * *
   * ****************** *
   **********************
*/
void loop() {
  //Estimation charge coeur
  unsigned long tps = millis();
  float deltaT = float(tps - previousLoop);
  previousLoop = tps;
  previousLoopMin = min(previousLoopMin, deltaT);
  previousLoopMin = previousLoopMin + 0.001;
  previousLoopMax = max(previousLoopMax, deltaT);
  previousLoopMax = previousLoopMax * 0.9999;
  previousLoopMoy = deltaT * 0.001 + previousLoopMoy * 0.999;

  //Gestion des serveurs
  //********************
  ArduinoOTA.handle();
  Debug.handle();
  server.handleClient();

  //Archivage et envois des mesures périodiquement
  //**********************************************
  if (EnergieActiveValide) {
    if (tps - previousHistoryMillis >= 300000) {  //Historique consommation par pas de 5mn
      previousHistoryMillis = tps;
      tabPw_Maison_5mn[IdxStockPW] = PuissanceS_M - PuissanceI_M;
      tabPw_Triac_5mn[IdxStockPW] = PuissanceS_T - PuissanceI_T;
      tabTemperature_5mn[IdxStockPW] = int(temperature);
      IdxStockPW = (IdxStockPW + 1) % 600;
    }

    if (tps - previousTimer2sMillis >= 2000) {
      previousTimer2sMillis = tps;
      tabPw_Maison_2s[IdxStock2s] = PuissanceS_M - PuissanceI_M;
      tabPw_Triac_2s[IdxStock2s] = PuissanceS_T - PuissanceI_T;
      tabPva_Maison_2s[IdxStock2s] = PVAS_M - PVAI_M;
      tabPva_Triac_2s[IdxStock2s] = PVAS_T - PVAI_T;
      IdxStock2s = (IdxStock2s + 1) % 300;
      envoiAuMQTT();
      JourHeureChange();
      EnergieQuotidienne();
    }

    if (tps - previousOverProdMillis >= 200) {
      previousOverProdMillis = tps;
      GestionOverproduction();
    }
  }
  if (tps - previousLEDsMillis >= 50) {
    previousLEDsMillis = tps;
    Gestion_LEDs();
  }
  //Vérification du WIFI
  //********************
  if (tps - previousWifiMillis > 30000) {  //Test présence WIFI toutes les 30s et autres
    previousWifiMillis = tps;
    if (WiFi.waitForConnectResult(10000) != WL_CONNECTED) {
      StockMessage("WIFI Connection Failed! #" + String(WIFIbug));
      WIFIbug++;
      if (WIFIbug > 2) {
        ESP.restart();
      }
    } else {
      WIFIbug = 0;
    }

    if (WiFi.getMode() != WIFI_STA) {
      Serial.print("Access Point Mode. IP address: ");
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.print("Niveau Signal WIFI:");
      Serial.println(WiFi.RSSI());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("WIFIbug:");
      Serial.println(WIFIbug);
      Debug.print("Niveau Signal WIFI:");
      Debug.println(WiFi.RSSI());
      Debug.print("WIFIbug:");
      Debug.println(WIFIbug);
      Serial.println("Charge Lecture RMS (coeur 0) en ms - Min : " + String(int(previousTimeRMSMin)) + " Moy : " + String(int(previousTimeRMSMoy)) + "  Max : " + String(int(previousTimeRMSMax)));
      Debug.println("Charge Lecture RMS (coeur 0) en ms - Min : " + String(int(previousTimeRMSMin)) + " Moy : " + String(int(previousTimeRMSMoy)) + "  Max : " + String(int(previousTimeRMSMax)));
      Serial.println("Charge Boucle générale (coeur 1) en ms - Min : " + String(int(previousLoopMin)) + " Moy : " + String(int(previousLoopMoy)) + "  Max : " + String(int(previousLoopMax)));
      Debug.println("Charge Boucle générale (coeur 1) en ms - Min : " + String(int(previousLoopMin)) + " Moy : " + String(int(previousLoopMoy)) + "  Max : " + String(int(previousLoopMax)));
    }
    int T = int(millis() / 1000);
    float DureeOn = float(T) / 3600;
    Serial.println("ESP32 ON depuis : " + String(DureeOn) + " heures");
    Debug.println("ESP32 ON depuis : " + String(DureeOn) + " heures");
    //Temperature
    LectureTemperature();
    JourHeureChange();
    Call_EDF_data();
    int Ltarf = 0;  //Code binaire Tarif
    if (LTARF.indexOf("PLEINE") >= 0) Ltarf += 1;
    if (LTARF.indexOf("CREUSE") >= 0) Ltarf += 2;
    if (LTARF.indexOf("BLEU") >= 0) Ltarf += 4;
    if (LTARF.indexOf("BLANC") >= 0) Ltarf += 8;
    if (LTARF.indexOf("ROUGE") >= 0) Ltarf += 16;
    LTARFbin = Ltarf;
  }
  if ((tps - startMillis) > 180000 && WiFi.getMode() != WIFI_STA) {  //Connecté en  Access Point depuis 3mn. Pas normal
    Serial.println("Pas connecté en WiFi mode Station. Redémarrage");
    delay(5000);
    ESP.restart();
  }

}

// ************
// *  ACTIONS *
// ************
void GestionOverproduction() {
  float SeuilPw;
  float MaxTriacPw;
  float GainBoucle;
  int Type_En_Cours = 0;
  bool lissage = false;
  //Puissance est la puissance en entrée de maison. >0 si soutire. <0 si injecte
  //Cas du Triac. Action 0
  float Puissance = float(PuissanceS_M - PuissanceI_M);
  if (NbActions == 0) LissageLong = true;  //Cas d'un capteur seul et actions déporté sur autre ESP
  for (int i = 0; i < NbActions; i++) {
    Actif[i] = LesActions[i].Actif;
    if (Actif[i] >= 2) lissage = true;                                                    //En RAM
    Type_En_Cours = LesActions[i].TypeEnCours(HeureCouranteDeci, temperature, LTARFbin);  //0=NO,1=OFF,2=ON,3=PW,4=Triac
    if (Actif[i] > 0 && Type_En_Cours > 1 && DATEvalid) {                                 // On ne traite plus le NO
      if (Type_En_Cours == 2) {
        RetardF[i] = 0;
      } else {  // 3 ou 4
        SeuilPw = float(LesActions[i].Valmin(HeureCouranteDeci));
        MaxTriacPw = float(LesActions[i].Valmax(HeureCouranteDeci));
        GainBoucle = float(LesActions[i].Reactivite);  //Valeur stockée dans Port
        if (Actif[i] == 1 && i > 0) {                                            //Les relais en On/Off
          if (Puissance > MaxTriacPw) { RetardF[i] = 100; }                      //OFF
          if (Puissance < SeuilPw) { RetardF[i] = 0; }                           //On
        } else {                                                                 // le Triac ou les relais en sinus
          RetardF[i] = RetardF[i] + 0.0001;                                      //On ferme très légèrement si pas de message reçu. Sécurité
          RetardF[i] = RetardF[i] + (Puissance - SeuilPw) * GainBoucle / 10000;  // Gain de boucle de l'asservissement
          if (RetardF[i] < 100 - MaxTriacPw) { RetardF[i] = 100 - MaxTriacPw; }
          if (ITmode < 0 && i==0 ) RetardF[i] = 100;  //Triac pas possible sur synchro interne
        }
        if (RetardF[i] < 0) { RetardF[i] = 0; }
        if (RetardF[i] > 100) { RetardF[i] = 100; }
      }
    } else {
      RetardF[i] = 100;
    }
    Retard[i] = int(RetardF[i]);  //Valeure entiere pour piloter le Triac et les relais
    if (Retard[i] == 100) {       // Force en cas d'arret des IT
      LesActions[i].Arreter();
      PulseOn[i] = 0;  //Stop Triac ou relais
    } else {

      switch (Actif[i]) {  //valeur en RAM du Mode de regulation
        case 1:            //Decoupe Sinus pour Triac ou On/Off pour relais
          if (i > 0) LesActions[i].RelaisOn();
          break;
        case 2:  // Multi Sinus
          PulseOn[i] = tabPulseSinusOn[100 - Retard[i]];
          PulseTotal[i] = tabPulseSinusTotal[100 - Retard[i]];
          break;
        case 3:  // Train de Sinus
          PulseOn[i] = 100 - Retard[i];
          PulseTotal[i] = 99;  //Nombre impair pour éviter courant continu
          break;
      }
    }
  }
  LissageLong = lissage;
}

void InitGpioActions() {
  for (int i = 1; i < NbActions; i++) {
    LesActions[i].InitGpio();
    Gpio[i] = LesActions[i].Gpio;
    OutOn[i] = LesActions[i].OutOn;
    OutOff[i] = LesActions[i].OutOff;
  }
}
// ***********************************
// * Calage Zéro Energie quotidienne * -
// ***********************************

void EnergieQuotidienne() {
  if (DATEvalid && Source != "Ext") {
    if (Energie_M_Soutiree < EAS_M_J0 || EAS_M_J0 == 0) {
      EAS_M_J0 = Energie_M_Soutiree;
    }
    EnergieJour_M_Soutiree = Energie_M_Soutiree - EAS_M_J0;
    if (Energie_M_Injectee < EAI_M_J0 || EAI_M_J0 == 0) {
      EAI_M_J0 = Energie_M_Injectee;
    }
    EnergieJour_M_Injectee = Energie_M_Injectee - EAI_M_J0;
    if (Energie_T_Soutiree < EAS_T_J0 || EAS_T_J0 == 0) {
      EAS_T_J0 = Energie_T_Soutiree;
    }
    EnergieJour_T_Soutiree = Energie_T_Soutiree - EAS_T_J0;
    if (Energie_T_Injectee < EAI_T_J0 || EAI_T_J0 == 0) {
      EAI_T_J0 = Energie_T_Injectee;
    }
    EnergieJour_T_Injectee = Energie_T_Injectee - EAI_T_J0;
  }
}

// **************
// * Heure DATE * -
// **************
void time_sync_notification(struct timeval *tv) {
  Serial.println("Notification de l'heure ( time synchronization event ) ");
  DATEvalid = true;
  Serial.print("Sync time in ms : ");
  Serial.println(sntp_get_sync_interval());
  JourHeureChange();
  StockMessage("Réception de l'heure");
}

// ***************
// * Temperature *
// ***************
void LectureTemperature() {
  float temperature_brute = -127;
  ds18b20.requestTemperatures();
  temperature_brute = ds18b20.getTempCByIndex(0);
  if (temperature_brute < -20 || temperature_brute > 130) {  //Invalide. Pas de capteur ou parfois mauvaise réponse
    Serial.print("Mesure Température invalide ");

  } else {
    temperature = temperature_brute;
    Serial.print("Température : ");
    Serial.print(temperature);
    Serial.println("°C");
    Debug.print("Température : ");
    Debug.print(temperature);
    Debug.println("°C");
  }
}

//****************
//* Gestion LEDs *
//****************
void Gestion_LEDs() {
  int retard_min = 100;
  int retardI;
  cptLEDyellow++;

  if (WiFi.status() != WL_CONNECTED) {  // Attente connexion au Wifi
    if (WiFi.getMode() == WIFI_STA) {   // en  Station mode
      cptLEDyellow = (cptLEDyellow + 6) % 10;
      cptLEDgreen = cptLEDyellow;
    } else {  //AP Mode
      cptLEDyellow = cptLEDyellow % 10;
      cptLEDgreen = (cptLEDyellow + 5) % 10;
    }
  } else {
    for (int i = 0; i < NbActions; i++) {
      retardI = Retard[i];
      retard_min = min(retard_min, retardI);
    }
    if (retard_min < 100) {
      cptLEDgreen = int((cptLEDgreen + 1 + 8 / (1 + retard_min / 10))) % 10;
    } else {
      cptLEDgreen = 10;
    }
  }
  if (cptLEDyellow > 5) {
    digitalWrite(LedYellow, LOW);
  } else {
    digitalWrite(LedYellow, HIGH);
  }
  if (cptLEDgreen > 5) {
    digitalWrite(LedGreen, LOW);
  } else {
    digitalWrite(LedGreen, HIGH);
  }
}
//*************
//* Test Pmax *
//*************
float PfloatMax(float Pin) {
  float P = max(-PmaxReseau, Pin);
  P = min(PmaxReseau, P);
  return P;
}
int PintMax(int Pin) {
  int M = int(PmaxReseau);
  int P = max(-M, Pin);
  P = min(M, P);
  return P;
}
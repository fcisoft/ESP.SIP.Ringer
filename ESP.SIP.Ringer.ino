
#include "config.h"
#include <WiFiManager.h>
#include "ArduinoSIP.h"
#include "Suoneria.h";
#include <Ticker.h>
#include <LittleFS.h>

Ticker timer;
WiFiManager wifiManager;

char acSipIn[2048];
char acSipOut[2048];
Sip aSip(acSipOut, sizeof(acSipOut));



// Sip parameters
String SipIP   = "sip.server";        
String SipPORT = "5060";                 
String SipUSER = "SIPRinger";           
String SipPW   = "SIPRinger.Password";   


Suoneria ring(speakerPin, ledPin);


char lastCallfrom[256];
char WiFiIP[16];


bool suona = false;
unsigned long tick=0;
byte c=0;

bool shouldSaveConfig = false;


void ICACHE_RAM_ATTR onTime() {
  digitalWrite(ledPin,!digitalRead(ledPin));
  timer1_write(2500000);//12us
}

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void callCallback(const char * from){
  Serial.print("Received a call from: ");
  Serial.println(from);
  digitalWrite(ledPin,true);
  ring.startRing();
}

void cancelCallback(const char * from){
  Serial.print("Call canceled from: ");
  Serial.println(from);
  digitalWrite(ledPin,false);
  ring.stopRing();
}


void carica(){
  Serial.println("Carico i dati dal  FS");
  File file = LittleFS.open("/config.txt", "r");
  if(!file){
    Serial.println("No Saved Data!");
    return;
  }
  SipIP=file.readStringUntil('\n');
  SipPORT=file.readStringUntil('\n');
  SipUSER=file.readStringUntil('\n');
  SipPW=file.readStringUntil('\n');
  file.close();
  Serial.print("Saved Data :\n");
  Serial.println(SipIP+"\n"+SipPORT+"\n"+SipUSER+"\n"+SipPW);
}



void salva(){
  LittleFS.remove("/config.txt"); 
  delay(10);
  File file = LittleFS.open("/config.txt", "w");
  String s=SipIP+"\n"+SipPORT+"\n"+SipUSER+"\n"+SipPW;
  file.print(s);
  delay(10);
  file.close();
  Serial.println("Write successful");
}

void setup(){
  Serial.begin(115200);
  pinMode(resetPin, INPUT_PULLUP);
  pinMode(ledPin,OUTPUT);
  WiFi.mode(WIFI_STA);  
  digitalWrite(ledPin,LOW);
  timer1_attachInterrupt(onTime); // Add ISR Function
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(2500000); // 2500000 / 5 ticks per us from TIM_DIV16 == 500,000 us interval 
  Serial.println("\n\n");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  carica(); 
 
  WiFiManagerParameter sipServer("sipServer", "VoIP IP server", SipIP.c_str(), 15);
  wifiManager.addParameter(&sipServer);
  WiFiManagerParameter sipPort("port", "port", SipPORT.c_str(), 6);
  wifiManager.addParameter(&sipPort);
  WiFiManagerParameter sipUsernname("userId", "SIP Username", SipUSER.c_str(), 40);
  wifiManager.addParameter(&sipUsernname);
  WiFiManagerParameter sipPassword("password", "SIP Password", SipPW.c_str(), 40);
  wifiManager.addParameter(&sipPassword);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setDebugOutput(true);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalTimeout(180);
  if (digitalRead(resetPin)==LOW){
    wifiManager.startConfigPortal("ESPSipRinger");
  }else{
    wifiManager.autoConnect("ESPSipRinger");
  }
  timer1_detachInterrupt();
  timer1_disable();   
  digitalWrite(ledPin,HIGH);
  String ip = WiFi.localIP().toString();
  Serial.println("Connesso, ip:"+ip);
  SipIP=sipServer.getValue();
  SipPORT=sipPort.getValue();
  SipUSER=sipUsernname.getValue();
  SipPW=sipPassword.getValue();
  if (shouldSaveConfig){
    Serial.println("Configurazione cambiata. Salvo");
    salva();
  }
  Serial.println("Server SIP:"+SipIP);
  Serial.println("SIP Port:"+SipPORT);
  Serial.println("SIP Username:"+SipUSER);
  Serial.println("SIP Password:"+SipPW);
  digitalWrite(ledPin,HIGH);
  ip.toCharArray(WiFiIP, 16);
  aSip.Init(SipIP.c_str(), SipPORT.toInt(), WiFiIP, SipPORT.toInt(), SipUSER.c_str(), SipPW.c_str(), 20, 1);
  aSip.Subscribe();
  aSip.setCallCallback(callCallback);
  aSip.setCancelCallback(cancelCallback);
}


void loop(void){
  aSip.loop(acSipIn, sizeof(acSipIn));
  ring.handle();
}

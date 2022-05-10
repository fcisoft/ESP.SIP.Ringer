#include "config.h"
#include <WiFiManager.h>
#include "ArduinoSIP.h"
#include "Suoneria.h";
#include <Ticker.h>

Ticker timer;
WiFiManager wifiManager;

char acSipIn[2048];
char acSipOut[2048];
Sip aSip(acSipOut, sizeof(acSipOut));



// Sip parameters
String SipIP   = "0.0.0.0";        
String SipPORT = "5060";                 
String SipUSER = "";           
String SipPW   = "";   


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

void setup(){
  pinMode(resetPin, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);  
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);
  Serial.begin(115200);
  timer1_attachInterrupt(onTime); // Add ISR Function
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(2500000); // 2500000 / 5 ticks per us from TIM_DIV16 == 500,000 us interval 
 
  WiFiManagerParameter sipServer("sipServer", "VoIP IP server", "0.0.0.0", 15);
  wifiManager.addParameter(&sipServer);
  WiFiManagerParameter sipPort("port", "port", "5060", 6);
  wifiManager.addParameter(&sipPort);
  WiFiManagerParameter sipUsernname("userId", "SIP Username", "", 40);
  wifiManager.addParameter(&sipUsernname);
  WiFiManagerParameter sipPassword("password", "SIP Password", "", 40);
  wifiManager.addParameter(&sipPassword);
  
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
  
  Serial.println("Server SIP:"+SipIP);
//  Serial.println("SIP Port:"+SipPORT);
//  Serial.println("SIP Username:"+SipUSER);
//  Serial.println("SIP Password:"+SipPW);
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

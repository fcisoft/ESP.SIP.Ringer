#include "config.h"
#include <ESP8266WiFi.h>
#include "ArduinoSIP.h"
#include "Suoneria.h";


char acSipIn[2048];
char acSipOut[2048];
Sip aSip(acSipOut, sizeof(acSipOut));

Suoneria ring(speakerPin);


char lastCallfrom[256];
char WiFiIP[16];


bool suona = false;
unsigned long tick=0;
byte c=0;

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
  Serial.begin(115200);
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,HIGH);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  String ip = WiFi.localIP().toString();
  Serial.println("connesso, ip:"+ip);
  digitalWrite(ledPin,LOW);
  ip.toCharArray(WiFiIP, 16);
  aSip.Init(SipIP, SipPORT, WiFiIP, SipPORT, SipUSER, SipPW, SipEXPIRES, 1);
  aSip.Subscribe();
  aSip.setCallCallback(callCallback);
  aSip.setCancelCallback(cancelCallback);
}


void loop(void){
  aSip.loop(acSipIn, sizeof(acSipIn));
  ring.handle();
}

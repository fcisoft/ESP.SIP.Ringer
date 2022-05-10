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
  /* Dividers:
    TIM_DIV1 = 0,   //80MHz (80 ticks/us - 104857.588 us max)
    TIM_DIV16 = 1,  //5MHz (5 ticks/us - 1677721.4 us max)
    TIM_DIV256 = 3  //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
  Reloads:
    TIM_SINGLE  0 //on interrupt routine you need to write a new value to start the timer again
    TIM_LOOP  1 //on interrupt the counter will start with the same value again
  */
  
  // Arm the Timer for our 0.5s Interval
  timer1_write(2500000); // 2500000 / 5 ticks per us from TIM_DIV16 == 500,000 us interval 
    
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
  Serial.println("connesso, ip:"+ip);
  digitalWrite(ledPin,HIGH);
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

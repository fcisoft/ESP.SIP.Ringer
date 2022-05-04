#include "Suoneria.h"

Suoneria::Suoneria (byte piedino){
      pin=piedino;
      init();
}

void Suoneria::init(){
  pinMode(pin, OUTPUT);
  suona=false;
}
  
void Suoneria::startRing() {
  suona=true;
  t=millis();
}

void Suoneria::stopRing() {
  suona=false;
  c=0;
  t=millis();
}

void Suoneria::handle(){
  if (suona){
    if (c<7){
        if (millis()<t+100){
          tone(pin,1400,1);
        }else if (millis()<t+100+50){
          tone(pin,1000,1);
        }else{
          t=millis();
          c++;
        }
     }else{
       if (millis()>t+800){
         c=0;
         t=millis();
       }
     }
   }
}

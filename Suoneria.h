#ifndef SUONERIA_H
#define SUONERIA_H

#include <Arduino.h>

class Suoneria {
  
  private:
    byte pin;
    byte ledPin;
    unsigned long t;
    bool suona;
    byte c;

  public:
    Suoneria(byte pin, byte ledPin);
    void init();
    void handle();
    void startRing();
    void stopRing();
};

#endif

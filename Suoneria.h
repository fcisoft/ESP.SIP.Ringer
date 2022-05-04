#ifndef SUONERIA_H
#define SUONERIA_H

#include <Arduino.h>

class Suoneria {
  
  private:
    byte pin;
    unsigned long t;
    bool suona;
    byte c;

  public:
    Suoneria(byte pin);
    void init();
    void handle();
    void startRing();
    void stopRing();
};

#endif

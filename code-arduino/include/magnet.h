#ifndef MAGNET_H
#define MAGNET_H

#include <Arduino.h>
#include "board.h"

class Magnet
{
private:
    Servo servo;
    unsigned long lastDrop = 0;

public:
    void init();
    void extend();
    void retract();

    bool isExtended();
};

#endif
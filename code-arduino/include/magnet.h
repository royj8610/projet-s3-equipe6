#ifndef MAGNET_H
#define MAGNET_H

#include <Arduino.h>
#include "board.h"

class Magnet
{
private:
    Servo servo;
    unsigned long lastDrop = 0;
    bool extended = false;

public:
    void init();
    void detach();
    void update();

    bool isExtended();
};

#endif
#include "magnet.h"

#define RANGE_DEG 90
#define DELAY_MILLIS 200

void Magnet::init()
{
    servo.attach(SERVO_PIN);
    servo.write(SERVO_OFFSET);
    extended = false;
}

void Magnet::detach()
{
    lastDrop = millis();
}

void Magnet::update()
{
    if (millis() - lastDrop < DELAY_MILLIS)
    {
        if (!extended)
        {
            servo.write(SERVO_OFFSET + RANGE_DEG);
            extended = true;
        }
    }
    else if (extended)
    {
        servo.write(SERVO_OFFSET);
        extended = false;
    }
}

bool Magnet::isExtended()
{
    return extended;
}
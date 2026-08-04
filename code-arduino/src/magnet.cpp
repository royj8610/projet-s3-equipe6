#include "magnet.h"

#define RANGE_DEG 90
#define DELAY_MILLIS 200

void Magnet::init()
{
    servo.attach(SERVO_PIN);
    servo.write(SERVO_OFFSET);
}

void Magnet::extend()
{
    lastDrop = millis();
    servo.write(SERVO_OFFSET + RANGE_DEG);
    extended = true;
}

void Magnet::retract()
{
    servo.write(SERVO_OFFSET);
    extended = false;
}

bool Magnet::isExtended()
{
    return (millis() - lastDrop) > DELAY_MILLIS && extended;
}
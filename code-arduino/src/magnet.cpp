#include <Arduino.h>
#include "board.h"
#include "magnet.h"

#define RANGE_DEG 90
#define DELAY_MILLIS 300

Servo servo;

void magnetInit() {
    servo.attach(SERVO_PIN);

    servo.write(SERVO_OFFSET);
}

void magnetDetach() {
    servo.write(RANGE_DEG + SERVO_OFFSET);

    delay(DELAY_MILLIS);

    servo.write(SERVO_OFFSET);
}
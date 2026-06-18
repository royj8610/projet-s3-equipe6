// #include <Arduino.h>

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   int result = myFunction(2, 3);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }

#include <Arduino.h>
#include "inertie_centrale.h"

void setup() {
    Serial.begin(9600);

    while (!Serial) {
        ;
    }

    imu_init();
}

void loop() {
    imu_afficherAcceleration();
    delay(300);
}
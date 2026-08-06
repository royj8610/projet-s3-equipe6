// #include <Arduino.h>
// #include <Wire.h>
// #include <MPU6050.h>
// #include "inertie_centrale.h"

// // forcer l'adresse I2C a etre 0x69
// #define IMU_ADDRESS 0x69

// // conversion par defaut de l'accelerometre :
// // plage ±2g -> 16384 LSB = 1 g
// #define ACCEL_SCALE 16384.0

// // Constante de gravite : 1 g = 9.81 m/s^2
// #define G_TO_MS2 9.81

// // objet IMU avec l'adresse 0x69
// MPU6050 imu(IMU_ADDRESS);

// // variables globales pour stocker les accelerations en m/s^2
// double accel_X = 0.0;
// double accel_Y = 0.0;
// double accel_Z = 0.0;

// void imu_init() {
//     Serial.println("Initialisation de la centrale inertielle...");
//     Serial.flush();

//     Wire.begin();

//     // initialiser le capteur
//     imu.initialize();

//     Serial.println("Initialisation terminee. Lecture de l'acceleration...");
// }

// void imu_lireAcceleration() {
//     int16_t ax;
//     int16_t ay;
//     int16_t az;

//     // lecture brute de l'accelerometre
//     imu.getAcceleration(&ax, &ay, &az);

//     // conversion des valeurs brutes en m/s^2
//     accel_X = (ax / ACCEL_SCALE) * G_TO_MS2;
//     accel_Y = (ay / ACCEL_SCALE) * G_TO_MS2;
//     accel_Z = (az / ACCEL_SCALE) * G_TO_MS2;
// }

// void imu_afficherAcceleration() {

//     imu_lireAcceleration();

//     Serial.print("Accel [m/s^2] X=");
//     Serial.print(accel_X);

//     Serial.print(" Y=");
//     Serial.print(accel_Y);

//     Serial.print(" Z=");
//     Serial.println(accel_Z);
// }

// double imu_getAccelX() {
//     imu_lireAcceleration();
//     return accel_X;
// }

// double imu_getAccelY() {
//     imu_lireAcceleration();
//     return accel_Y;
// }

// double imu_getAccelZ() {
//     imu_lireAcceleration();
//     return accel_Z;
// }
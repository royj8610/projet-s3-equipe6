#ifndef PROJECT_PendulumEncoder_H
#define PROJECT_PendulumEncoder_H

#include <Arduino.h>
#include <AS5047P.h>

class PendulumEncoder
{
public:
    explicit PendulumEncoder(uint8_t chipSelectPin);

    bool init();

    double readAngle();
    double readAngleRad();

    bool setZero();
    bool resetZero();

private:
    AS5047P _sensor;
    bool _initialized = false;

    bool writeZeroPosition(uint16_t zeroPosition);
};

#endif // PendulumEncoder_H

// #pragma once
// #include <stdint.h>

// void PendulumEncoderInit();
// uint16_t PendulumEncoderReadRegister(uint16_t adress);
// float PendulumEncoderGetAngle();

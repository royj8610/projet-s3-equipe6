#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#include "board.h"

#define NODE_ID 0

enum ControlMode
{
    TORQUE = 1,
    POSITION = 3,
    VELOCITY = 2
};

enum InputMode
{
    TRAP_TRAJ = 5,
    PASSTHROUGH = 1
};

enum AxisState
{
    CLOSED_LOOP_CONTROL = 8,
    IDLE = 1
};

class Motor
{
private:
    MCP2515 mcp2515 = MCP2515(MCP2515_CS_PIN);
    SPIClass &spi = SPI;

    const float position_offset = 0;

    float position = 0;
    float velocity = 0;

    void sendCAN(uint32_t id, uint8_t *data, uint8_t len);

public:
    Motor();

    void setControllerMode(uint32_t control_mode, uint32_t input_mode);
    void setAxisState(uint32_t axis_state);
    void setPosition(float pos);
    void setVelocity(float vel);
    void setTorque(float torque);

    bool fetchEncoderEstimates(uint32_t timeout_ms = 50);
    float getPosition();
    float getVelocity();

    void init();
};

#endif
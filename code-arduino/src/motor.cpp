#include "motor.h"

Motor::Motor()
{
    mcp2515 = MCP2515(MCP2515_CS_PIN);
    spi = SPIClass(VSPI);
}

void Motor::sendCAN(uint32_t id, uint8_t *data, uint8_t len)
{
    struct can_frame frame;

    frame.can_id = id;
    frame.can_dlc = len;

    memcpy(frame.data, data, len);

    mcp2515.sendMessage(&frame);
}

bool Motor::fetchEncoderEstimates(uint32_t timeout_ms)
{
    // Send RTR request
    struct can_frame request;

    request.can_id = (NODE_ID << 5) | 0x09 | CAN_RTR_FLAG;
    request.can_dlc = 0;

    if (mcp2515.sendMessage(&request) != MCP2515::ERROR_OK)
    {
        return false;
    }

    // Wait for response
    uint32_t start = millis();

    struct can_frame response;

    while (millis() - start < timeout_ms)
    {
        if (mcp2515.readMessage(&response) == MCP2515::ERROR_OK)
        {
            uint32_t expected_id = (NODE_ID << 5) | 0x09;

            if (response.can_id == expected_id &&
                response.can_dlc == 8)
            {
                memcpy(&position,
                       response.data,
                       sizeof(float));

                memcpy(&velocity,
                       response.data + 4,
                       sizeof(float));

                return true;
            }
        }
    }

    return false;
}

void Motor::setControllerMode(uint32_t control_mode, uint32_t input_mode)
{
    struct
    {
        uint32_t control_mode;
        uint32_t input_mode;
    } msg;

    // Position control
    msg.control_mode = control_mode;

    // Passthrough
    msg.input_mode = input_mode;

    sendCAN(
        (NODE_ID << 5) | 0x0B,
        (uint8_t *)&msg,
        8);
}

void Motor::setAxisState(uint32_t axis_state)
{
    sendCAN(
        (NODE_ID << 5) | 0x07,
        (uint8_t *)&axis_state,
        4);
}

void Motor::setPosition(float pos)
{
    struct
    {
        float pos;
        float vel_ff;
        float torque_ff;

    } msg;

    msg.pos = pos;
    msg.vel_ff = 0;
    msg.torque_ff = 0;

    sendCAN(
        (NODE_ID << 5) | 0x0C,
        (uint8_t *)&msg,
        8);
}

void Motor::setVelocity(float vel)
{
    struct
    {
        float vel;
        float input_torque_ff;

    } msg;

    msg.vel = vel;
    msg.input_torque_ff = 0;

    sendCAN(
        (NODE_ID << 5) | 0x0D,
        (uint8_t *)&msg,
        8);
}

void Motor::setTorque(float torque)
{
    sendCAN(
        (NODE_ID << 5) | 0x0E,
        (uint8_t *)&torque,
        8);
}

float Motor::getPosition()
{
    return position;
}

float Motor::getVelocity()
{
    return velocity;
}

void Motor::init()
{
    spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, MCP2515_CS_PIN);

    mcp2515.reset();

    mcp2515.setBitrate(
        CAN_500KBPS,
        MCP_8MHZ);

    mcp2515.setNormalMode();
}
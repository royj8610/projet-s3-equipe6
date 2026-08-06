#if defined(BOARD_ESP32)
// Includes
#include <ESP32Servo.h>

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

#define SERVO_PIN 33
#define SERVO_OFFSET 10

#define AS5047P_CS_PIN 25 // Encoder chip select pin
#define MCP2515_CS_PIN 32 // CAN module chip select pin

#define INTERRUPT_PIN 4

#define MOTOR_SIGN 1.0

#define IS_ARDUINO false

#elif defined(BOARD_MEGA)
// Includes
#include <Servo.h>

#define SPI_SCK 52
#define SPI_MISO 50
#define SPI_MOSI 51

#define SERVO_PIN 2
#define SERVO_OFFSET 0

#define AS5047P_CS_PIN 46 // Encoder chip select pin
#define MCP2515_CS_PIN 47 // CAN module chip select pin

#define INTERRUPT_PIN 21

#define MOTOR_SIGN -1.0

#define IS_ARDUINO true

#else
// Throws a compilation error if neither board is selected
#error "Unsupported hardware target! Please select ESP32 or Arduino Mega 2560."
#endif
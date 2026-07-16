#if defined(BOARD_ESP32)
    // Includes
    #include <ESP32Servo.h>

    // Code specific to ESP32 runs here
    constexpr const char* BOARD_TYPE = "ESP32";

    #define SPI_SCK 18
    #define SPI_MISO 19
    #define SPI_MOSI 23

    #define SERVO_PIN 33
    #define SERVO_OFFSET 0

    #define AS5047P_CS_PIN 25

#elif defined(BOARD_MEGA)
    // Includes
    #include <Servo.h>

    // Code specific to Arduino Mega 2560 runs here
    constexpr const char* BOARD_TYPE = "Mega2560";

    #define SPI_SCK 52
    #define SPI_MISO 50
    #define SPI_MOSI 51

    #define SERVO_PIN 1 // TODO : Needs to be updated according to wiring
    #define SERVO_OFFSET 0

    #define AS5047P_CS_PIN 25 // TODO : Needs to be updated according to wiring

#else
    // Throws a compilation error if neither board is selected
    #error "Unsupported hardware target! Please select ESP32 or Arduino Mega 2560."
#endif
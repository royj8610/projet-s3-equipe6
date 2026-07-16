#include <SPI.h>
#include <AS5047P.h>

// CS pin 9, SPI speed default from library header (can pass a custom speed)
AS5047P as5047p(9);

// FUNCTION : Initializing the encoder
// RETURNS : None
void encoderAltInit() {

    Serial.begin(115200);
    while (!Serial){
        delay(10); // Wait for Serial to be ready
    }
    Serial.println(F("Initializing AS5047P sensor..."));
    // Initialize SPI via library and verify connectivity
    if (!as5047p.initSPI()) {
        Serial.println("AS5047P init failed. Check wiring and power.");
        while (true) { delay(1000); }
    }
    Serial.println(F("AS5047P sensor successfully initialized."));

}
// FUNCTION : Gets the angle from the encoder
// RETURNS : Float of the angle in degrees
float encoderAltGetAngle() {
    // Read 14-bit angle (degrees) with DAE compensation
    float deg = as5047p.readAngleDegree(true);
    return deg;
}
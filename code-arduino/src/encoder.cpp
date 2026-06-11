#include <SPI.h>
/* 
Par défaut: 
    MOSI = 11
    MISO = 12
    CLOCK = 13
*/

///////////////// INFO FOR ENCODER /////////////////////
/* 
mode=1
CPOL=0 (clock polarity)
CPHA=1 (clock phase)
MSB first
Max clock speed : 10 MHz
COMMAND FRAME
    15 >> parity bit
    14 >> 0:Write / 1=Read
    13:0 >> adress to read or write
    
READ DATA FRAME
    15 >> parity bit
    14 >> error frame (0: no command franme error / 1: error)
    13:0 >> data

WRITE DATA FRAME
    15 >> parity bit
    14 >> error frame (0: no command franme error / 1: error)
    13:0 >> data

    
angle adress : 0x3FFF
*/

//select le pin relié au chip_select de l'encoder
const int CS_encoderPin = 10; 
const int maxClockSpeed = 10000000;

void setup() {
  Serial.begin(115200);
  SPISettings(maxClockSpeed, MSBFIRST, SPI_MODE1);

  // definir le pin CS comme une sortie
  pinMode(CS_encoderPin, OUTPUT);
  // ecrire cs à high car c'est un active low
  digitalWrite(CS_encoderPin, HIGH);

  SPI.begin();
}


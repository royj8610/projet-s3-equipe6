// #include <SPI.h>
// /* 
// Par défaut: 
//     MOSI = 11
//     MISO = 12
//     CLOCK = 13
// */

// ///////////////// INFO FOR ENCODER /////////////////////
// /* 
// mode=1
// CPOL=0 (clock polarity)
// CPHA=1 (clock phase)
// MSB first
// Max clock speed : 10 MHz
// 1024 pulse per revolution

// COMMAND FRAME
//     15 >> parity bit (even)
//     14 >> 0=Write / 1=Read
//     13:0 >> adress to read or write
    
// READ DATA FRAME
//     15 >> parity bit (even)
//     14 >> error frame (0= no command franme error / 1= error)
//     13:0 >> data

// WRITE DATA FRAME (old content send on MISO)
//     15 >> parity bit (even)
//     14 >> 0 always low
//     13:0 >> data
  
// VOLATILE REGISTERS
//     0x0000 >> No operation. Reading that register counts as NOP (no operation)
//     0x0001 >> ERROR register
//         0000000000000100 >> parity error
//         0000000000000010 >> invalid command error
//         0000000000000001 >> framing error 
//     0x0003 >> Programming register
//     0x3FFC >> Diagnostic and AGC
//     0x3FFD >> CORDIC magnitude
//     0x3FFE >> Measured angle WITHOUT dynamic angle
//               error compensation
//     0x3FFF >> Measured angle WITH dynamic angle error
//               compensation
//         13:0 >> angle information

// SPI WRITE TRANSACTION
// 1. write command frame (14=low)
// 2. write data frame

// */

// //select le pin relié au chip_select de l'encoder
// const int CS_encoderPin = 10; 
// const long maxClockSpeed = 1000000;

// void encoderInit() {
//   Serial.begin(115200);
//   SPISettings(maxClockSpeed, MSBFIRST, SPI_MODE1);

//   // definir le pin CS comme une sortie
//   pinMode(CS_encoderPin, OUTPUT);
//   // ecrire cs à high car c'est un active low
//   digitalWrite(CS_encoderPin, HIGH);

//   SPI.begin();
// }

// uint16_t calcEvenParity(uint16_t value) {
//     /* 
//     calculates the parity of a 16 bit value and returns the parity bit as the LSB of a 16 bit value
//     */

//     // set parity to 0, need at least one value for XOR
//     uint16_t parity = 0;
//     /*
//     slides the value to the right by i, checks if LSB is 1, t
//     then iterates over all bits in value
//     */ 
//     for(int i=0; i<16; i++){
//         parity ^= (value>>i) & 1;
//     }
//     return parity;
// }

// uint16_t buildReadCommand(uint16_t adress){
//     // set R/W bit (bit 14) 0100000000000000
//     // | is a bitwise OR operator
    
//     uint16_t cmd = 0x4000 | adress;
//     uint16_t parity = calcEvenParity(cmd);
//     cmd |= (parity << 15); // adding parity bit in its place using OR
//     return cmd;
// }

// // FUNCTION : reads the angle register of the encoder at given hexadecimal adress
// // ARGUMENT :
// // RETURNS :
// uint16_t readRegister(uint16_t adress){

//     uint16_t cmd = buildReadCommand(adress);

//     SPI.beginTransaction(SPISettings(maxClockSpeed, MSBFIRST, SPI_MODE1));

//     ////////// FRAME 1 : send command (16 bit), in this case a read command //////////////
//     digitalWrite(CS_encoderPin, LOW); // activate slave
//     SPI.transfer16(cmd);
//     digitalWrite(CS_encoderPin, HIGH); // deactivate slave

//     /////////// FRAME 2 : get response (16 bit) //////////////////
//     digitalWrite(CS_encoderPin, LOW);
//     uint16_t registerData = SPI.transfer16(0x0000); // NOP
//     digitalWrite(CS_encoderPin, HIGH);

//     SPI.endTransaction();

//     return registerData;
// }

// uint16_t extractJustAngleData(uint16_t registerData){
//     uint16_t angleData = registerData & 0x3FFF;
//     return angleData;
// }

// float pulse2degree(uint16_t angleData){
//     float angle = angleData * 360.0f/1024.0f;
//     return angle;
// }

// // FUNCTION : gets the angle from the encoder
// // ARGUMENT : None
// // RETURNS : angle in degrees
// float encoderGetAngle() {
//     uint16_t registerData = readRegister(0x3FFF);
//     uint16_t angleData = extractJustAngleData(registerData);
//     float angle = pulse2degree(angleData);
//     Serial.print("angle : ");
//     Serial.print(angle);

//     return angle;
// }




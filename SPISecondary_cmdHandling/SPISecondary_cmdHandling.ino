/***********************************************
* @file  SPISecondary_cmdHandling.ino
* @brief Arduino Mega2560 acting as SPI secondary 
*        device to receive a command byte and respond 
*        accordingly. Modified example from: FastBit 
*        Embedded Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/
/* 
 * SPI pin numbers (new terminology):
 * SCK   13  // Serial Clock
 * SDI   12  // Serial Data In
 * SDO   11  // Serial Data Out
 * CS    10  // Chip Select. Arduino SPI pins respond only if CS pulled low by the primary
 */
#include <SPI.h>

#define NACK  0xA5
#define ACK   0xF5

//command codes
#define COMMAND_LED_CTRL          0x50
#define COMMAND_SENSOR_READ       0x51
#define COMMAND_LED_READ          0x52
#define COMMAND_PRINT             0x53
#define COMMAND_ID_READ           0x54

#define LED_ON     1
#define LED_OFF    0

//Arduino analog pins
#define ANALOG_PIN0   0
#define ANALOG_PIN1   1
#define ANALOG_PIN2   2
#define ANALOG_PIN3   3
#define ANALOG_PIN4   4

//SPI redefines
#define SDO           MOSI
#define SDI           MISO
#define CS            SS

const byte led = 9;           //Secondary LED digital I/O pin.
boolean ledState = HIGH;      //LED state flag.
uint8_t dataBuff[255];
uint8_t board_id[11] = "ARDUINOMEGA";

//Initialize SPI secondary
void SPI_SecondaryInit(void) { 
  //Initialize SPI pins
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SS, INPUT);
  
  //Configure SPI as secondary
  
  //Enable SPI as secondary.
  SPCR = (1 << SPE);
}

//Return SPDR Contents 
uint8_t SPI_SecondaryReceive(void) {
  /* Wait for reception complete */
  while(!(SPSR & (1 << SPIF)));
  
  /* Return Data Register */
  return SPDR;
}

//Send data byte
void SPI_SecondaryTransmit(uint8_t data) {
  /* Start transmission */
  SPDR = data;
  
  /* Wait for transmission complete */
  while(!(SPSR & (1 << SPIF)));
}

byte checkData(byte command) {
  return ACK;
}

void setup() {
  //Initialize Serial
  Serial.begin(9600);
  
  //Initialize secondary LED pin
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  //Initialize SPI secondary
  SPI_SecondaryInit();
  
  Serial.println("Secondary Initialized");
}

void loop() {
  byte data, command, len, ackornack = NACK;
  
  //1. Wait until CS is pulled low
  Serial.println("Secondary waiting for cs to be pulled low");
  while(digitalRead(CS));
  
  //2. Wait until rx buffer has a byte
  command = SPI_SecondaryReceive();
  ackornack = checkData(command);
  SPI_SecondaryTransmit(ackornack);  
  len = SPI_SecondaryReceive();   //dummy byte

  //COMMAND_LED_CTRL
  if(command == COMMAND_LED_CTRL) {
    //read 2 more bytes pin number and value 
    uint8_t pin = SPI_SecondaryReceive(); 
    uint8_t value = SPI_SecondaryReceive();
     
    Serial.println("RCVD:COMMAND_LED_CTRL");
    if(value == (uint8_t)LED_ON) {
      digitalWrite(pin, HIGH);
    } else if(value == (uint8_t)LED_OFF) {
      digitalWrite(pin, LOW);
    }
    
  //COMMAND_SENSOR_READ
  } else if(command == COMMAND_SENSOR_READ) {
    //read analog pin number 
    uint16_t aread;
    uint8_t pin = SPI_SecondaryReceive();
     
    //pinMode(pin+14, INPUT_PULLUP);
    uint8_t val;
    aread = analogRead(pin+14);
    val = map(aread, 0, 1023, 0, 255);
    
    SPI_SecondaryTransmit(val);
    val = SPI_SecondaryReceive(); //dummy read
    Serial.println("RCVD:COMMAND_SENSOR_READ");
    
  //COMMAND_LED_READ
  } else if(command == COMMAND_LED_READ) {
    uint8_t pin = SPI_SecondaryReceive(); 
    uint8_t val = digitalRead(pin);
    
    SPI_SecondaryTransmit(val);
    val = SPI_SecondaryReceive(); //dummy read
    Serial.println("RCVD:COMMAND_LED_READ");
    
  //COMMAND_PRINT  
  } else if(command == COMMAND_PRINT) {
    uint8_t len = SPI_SecondaryReceive(); 
    for(int i = 0; i < len; i++) {
      dataBuff[i] = SPI_SecondaryReceive();
    }
    Serial.println((char*)dataBuff);
    Serial.println("RCVD:COMMAND_PRINT");

  //COMMAND_ID_READ  
  } else if(command == COMMAND_ID_READ) {
    for(int i = 0; i < strlen(board_id); i++) {
      SPI_SecondaryTransmit(board_id[i]);
    }
    SPI_SecondaryReceive();
    Serial.println("RCVD:COMMAND_ID_READ");
  }
}

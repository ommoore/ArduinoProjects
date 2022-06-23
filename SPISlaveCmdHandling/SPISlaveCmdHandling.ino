/***********************************************
* @file  SPISlaveCmdHandling.ino
* @brief Arduino Mega2560 acting as SPI slave device to
*        receive a command byte and respond accordingly. 
*        Modified from example from: FastBit Embedded 
*        Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/
/* 
 * SPI pin numbers:
 * SCK   13  // Serial Clock
 * MISO  12  // Master In Slave Out
 * MOSI  11  // Master Out Slave In
 * SS    10  // Slave Select. Arduino SPI pins respond only if SS pulled low by the master
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

const byte led = 9;           //Slave LED digital I/O pin.
boolean ledState = HIGH;      //LED state flag.
uint8_t dataBuff[255];
uint8_t board_id[11] = "ARDUINOMEGA";

//Initialize SPI slave
void SPI_SlaveInit(void) { 
  //Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SS, INPUT);
  
  //Configure SPI as slave
  
  //Enable SPI as slave.
  SPCR = (1 << SPE);
}

//Return SPDR Contents 
uint8_t SPI_SlaveReceive(void) {
  /* Wait for reception complete */
  while(!(SPSR & (1 << SPIF)));
  
  /* Return Data Register */
  return SPDR;
}

//Send data byte
void SPI_SlaveTransmit(uint8_t data) {
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
  
  //Initialize slave LED pin
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  //Initialize SPI Slave
  SPI_SlaveInit();
  
  Serial.println("Slave Initialized");
}

void loop() {
  byte data, command, len, ackornack = NACK;
  
  //1. Wait until SS is pulled low
  Serial.println("Slave waiting for ss to be pulled low");
  while(digitalRead(SS));
  
  //2. Wait until rx buffer has a byte
  command = SPI_SlaveReceive();
  ackornack = checkData(command);
  SPI_SlaveTransmit(ackornack);  
  len = SPI_SlaveReceive();   //dummy byte

  //COMMAND_LED_CTRL
  if(command == COMMAND_LED_CTRL) {
    //read 2 more bytes pin number and value 
    uint8_t pin = SPI_SlaveReceive(); 
    uint8_t value = SPI_SlaveReceive();
     
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
    uint8_t pin = SPI_SlaveReceive();
     
    //pinMode(pin+14, INPUT_PULLUP);
    uint8_t val;
    aread = analogRead(pin+14);
    val = map(aread, 0, 1023, 0, 255);
    
    SPI_SlaveTransmit(val);
    val = SPI_SlaveReceive(); //dummy read
    Serial.println("RCVD:COMMAND_SENSOR_READ");
    
  //COMMAND_LED_READ
  } else if(command == COMMAND_LED_READ) {
    uint8_t pin = SPI_SlaveReceive(); 
    uint8_t val = digitalRead(pin);
    
    SPI_SlaveTransmit(val);
    val = SPI_SlaveReceive(); //dummy read
    Serial.println("RCVD:COMMAND_LED_READ");
    
  //COMMAND_PRINT  
  } else if(command == COMMAND_PRINT) {
    uint8_t len = SPI_SlaveReceive(); 
    for(int i = 0; i < len; i++) {
      dataBuff[i] = SPI_SlaveReceive();
    }
    Serial.println((char*)dataBuff);
    Serial.println("RCVD:COMMAND_PRINT");

  //COMMAND_ID_READ  
  } else if(command == COMMAND_ID_READ) {
    for(int i = 0; i < strlen(board_id); i++) {
      SPI_SlaveTransmit(board_id[i]);
    }
    SPI_SlaveReceive();
    Serial.println("RCVD:COMMAND_ID_READ");
  }
}

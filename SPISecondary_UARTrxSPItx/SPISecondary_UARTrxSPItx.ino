/***********************************************
* @file  SPISecondary_UARTrxSPItx.ino
* @brief Arduino Mega2560 acting as SPI secondary 
*        device to read Serial UART data into buffer 
*        and retransmit over SPI. Modified example 
*        from: FastBit Embedded Brain Academy (Udemy) 
*        course.
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

#define MAX_LEN 500

//SPI redefines
#define SDO           MOSI
#define SDI           MISO
#define CS            SS

const byte led = 9;           //Secondary LED digital I/O pin
bool msgComplete = false;
uint8_t userBuffer[MAX_LEN];
uint32_t cnt = 0;

//Initialize SPI secondary
void SPI_SecondaryInit(void) { 
  //Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(SDO, INPUT);
  pinMode(SDI, OUTPUT);
  pinMode(CS, INPUT);
  
  //Enable SPI as secondary.
  SPCR = (1 << SPE);
}

//Return SPDR contents 
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

void notify_controller(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delayMicroseconds(50);
  digitalWrite(led, LOW);
}

void setup() {
  //Initialize Serial
  Serial.begin(9600);

  //Initialize secondary LED pin
  pinMode(led, INPUT_PULLUP);
  //digitalWrite(8,LOW);
  
  //Initialize SPI secondary
  SPI_SecondaryInit();

  Serial.println("Secondary Initialized");
}

void loop() {
  Serial.println("Type anything and send...");
  
  while(!msgComplete){
    if(Serial.available()) {
      //Read a byte of incoming serial data
      char readByte = (char)Serial.read();
      
      //Fill buffer with received bytes
      userBuffer[cnt++] = readByte;
      if(readByte == '\r' || ( cnt == MAX_LEN)){
        msgComplete = true;
        userBuffer[cnt-1] = '\0';  //replace '\r' with '\0'
      }
    }
  }
  
  Serial.println("Your message...");
  Serial.println((char*)userBuffer);
  
  notify_controller();

  /* Transmit the user buffer over SPI */
  for(uint32_t i = 0; i < cnt; i++) {
    SPI_SecondaryTransmit(userBuffer[i]);
  }
  cnt = 0;
  msgComplete = false;
  Serial.println("Message sent...");

  while(!digitalRead(CS));
  Serial.println("Master ends communication");
}

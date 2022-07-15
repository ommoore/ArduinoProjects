/***********************************************
* @file  SPISecondary_rxString.ino
* @brief Arduino Mega2560 acting as SPI secondary 
*        device to receive a string, or collection 
*        of bytes, from primary. Modified example 
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
#include <stdint.h>
  
//SPI redefines
#define SDO           MOSI
#define SDI           MISO
#define CS            SS

char dataBuff[500];

//Initialize SPI secondary.
void SPI_SecondaryInit(void) { 
  //Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(SDO, INPUT);
  pinMode(SDI, OUTPUT);
  pinMode(CS, INPUT);
  
  //Configure SPI as secondary
  
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
void SPI_SecondaryTransmit(char data) {
  /* Start transmission */
  SPDR = data;
  
  /* Wait for transmission complete */
  while(!(SPSR & (1 << SPIF)));
}
  
void setup() {
  //Initialize serial communication 
  Serial.begin(9600); 
  
  //Initialize SPI secondary.
  SPI_SecondaryInit();
  Serial.println("Secondary Initialized");
}

void loop() {
  uint32_t i = 0;
  uint16_t dataLen = 0;
  
  Serial.println("Secondary waiting for cs to go low");
  while(digitalRead(CS));

  dataLen = SPI_SecondaryReceive();
  for(i = 0 ; i < dataLen ; i++ ) {
    dataBuff[i] =  SPI_SecondaryReceive();
  }

  //Serial.println(String(i, HEX));
  dataBuff[i] = '\0';
  
  Serial.println("Rcvd:");
  Serial.println(dataBuff);
  Serial.print("Length:");
  Serial.println(dataLen);
}

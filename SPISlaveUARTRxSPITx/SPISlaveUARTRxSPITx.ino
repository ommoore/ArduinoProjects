/***********************************************
* @file  SPISlaveUARTRxSPITx.ino
* @brief Arduino Mega2560 acting as SPI slave device to
*        read Serial UART data into buffer and retransmit 
*        over SPI. Modified from example from: FastBit 
*        Embedded Brain Academy (Udemy) course.
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

#define MAX_LEN 500

const byte led = 9;           //Slave LED digital I/O pin
bool msgComplete = false;
uint8_t userBuffer[MAX_LEN];
uint32_t cnt = 0;

//Initialize SPI slave
void SPI_SlaveInit(void) { 
  //Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SS, INPUT);
  
  //Enable SPI as slave.
  SPCR = (1 << SPE);
}

//Return SPDR contents 
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

void notify_controller(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delayMicroseconds(50);
  digitalWrite(led, LOW);
}

void setup() {
  //Initialize Serial
  Serial.begin(9600);

  //Initialize slave LED pin
  pinMode(led, INPUT_PULLUP);
  //digitalWrite(8,LOW);
  
  //Initialize SPI Slave
  SPI_SlaveInit();

  Serial.println("Slave Initialized");
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
    SPI_SlaveTransmit(userBuffer[i]);
  }
  cnt = 0;
  msgComplete = false;
  Serial.println("Message sent...");

  while(!digitalRead(SS));
  Serial.println("Master ends communication");
}

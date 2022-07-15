/***********************************************
* @file  I2CPrimary_rxString.ino
* @brief Arduino Mega2560 acting as I2C primary device 
*        to receive a string, or collection of bytes, 
*        from secondary. Modified example from: FastBit 
*        Embedded Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/
//Wire (I2C) Primary receiver
//Arduino Mega2560 - [A4(SDA), A5(SCL)] or D20(SDA), D21(SCL)

#include <Wire.h>

#define SECONDARY_ADDR 0x68

int LED = 13;
uint8_t rcv_buf[32];
int data_len = 0;

void setup() {
  Serial.begin(9600);
    
  //Define the LED pin as output
  pinMode(LED, OUTPUT);
  
  //Join I2C bus (address optional for primary)
  Wire.begin(); 
}

void loop() {
  Serial.println("Arduino Primary");
  Serial.println("Send character \"s\" to begin");
  Serial.println("-----------------------------");

  //Wait until byte received
  while(!Serial.available());
  char in_read = Serial.read();

  //Wait until 's' is received
  while(in_read != 's');

  Serial.println("Starting..");

  Wire.beginTransmission(SECONDARY_ADDR);
  Wire.write(0x51);   //Send this command to read the length
  Wire.endTransmission();

  Wire.requestFrom(SECONDARY_ADDR, 1); //Request the transmitted bytes

  if(Wire.available()) {   
    data_len = Wire.read(); //Read data
  }
  
  Serial.print("Data Length:");
  Serial.println(String(data_len, DEC));

  Wire.beginTransmission(SECONDARY_ADDR);
  Wire.write(0x52); //Send this command to ask data
  Wire.endTransmission();

  Wire.requestFrom(SECONDARY_ADDR, data_len);

  uint32_t i = 0;
  for(i = 0; i <= data_len; i++) {
    if(Wire.available()) {
      rcv_buf[i] = Wire.read(); //Read data 
    }
  }
  rcv_buf[i] = '\0';

  Serial.print("Data:");
  Serial.println((char*)rcv_buf);
  Serial.println("*********************END*********************");
}

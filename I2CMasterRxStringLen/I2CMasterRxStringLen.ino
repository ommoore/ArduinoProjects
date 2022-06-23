/***********************************************
* @file  I2CMasterRxStringLen.ino
* @brief Arduino Mega2560 acting as I2C master device to
*        receive a variable-length string from slave. 
*        Modified from example from: FastBit Embedded 
*        Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/
//Wire (I2C) Master receiver
//Arduino Mega2560 - [A4(SDA), A5(SCL)] or D20(SDA), D21(SCL)

#include <Wire.h>

#define SLAVE_ADDR 0x68

int LED = 13;
uint8_t rcv_buf[512];
uint32_t data_len = 0, w_ptr = 0;

void setup() {
  Serial.begin(9600);
    
  //Define the LED pin as output
  pinMode(LED, OUTPUT);
  
  //Join I2C bus (address optional for master)
  Wire.begin(); 
}

void loop() {
  uint32_t rem_len = 0,last_read = 0;
  
  Serial.println("Arduino Master");
  Serial.println("Send character \"s\" to begin");
  Serial.println("-----------------------------");

  //Wait until byte received
  while(!Serial.available());
  char in_read = Serial.read();

  //Wait until 's' is received
  while(in_read != 's');

  Serial.println("Starting..");

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x51);   //Send this command to read the length
  Wire.endTransmission();

  Wire.requestFrom(SLAVE_ADDR, 4);  //Request the transmitted bytes
  for(uint32_t len = 0; len < 4; len++) {
    if(Wire.available()) { 
      uint32_t data = (uint32_t)Wire.read();
      data_len |= (data << (8 * len)); //Read data 
    }
  }
  
  //data_len = 0x2F8;
  Serial.print("Data Length:");
  Serial.println(data_len);

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0x52); //Send this command to ask data
  Wire.endTransmission();

  rem_len = data_len;
  while(rem_len > 0) {
    if(rem_len <= 32) {
      Wire.requestFrom(SLAVE_ADDR, rem_len);
      last_read = rem_len;
      rem_len = 0;
    } else {
      Wire.requestFrom(SLAVE_ADDR, 32);
      last_read = 32;
      rem_len -= 32;
    }
  
    uint32_t i = 0;
    for(i = 0; i <= last_read; i++) {
      if(Wire.available()) {
        rcv_buf[w_ptr++] = Wire.read(); //Read data 
      }
    }
  }
 
  rcv_buf[w_ptr] = '\0';
  w_ptr = 0;
  Serial.print("Data:");
  Serial.println((char*)rcv_buf);
  Serial.println("*********************END*********************");
}

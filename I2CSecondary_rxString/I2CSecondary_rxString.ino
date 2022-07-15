/***********************************************
* @file  I2CSecondary_rxString.ino
* @brief Arduino Mega2560 acting as I2C secondary 
*        device to receive a string, or collection 
*        of bytes, from primary. Modified example 
*        from: FastBit Embedded Brain Academy (Udemy) 
*        course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/
//Wire (I2C) Secondary receiver
//Arduino Mega2560 - [A4(SDA), A5(SCL)] or D20(SDA), D21(SCL)

#include <Wire.h>

#define MY_ADDR   0x68

int LED = 13;
char rx_buffer[32];
uint32_t cnt = 0;
uint8_t message[50];

void setup() {
  Serial.begin(9600);
  
  //Define the LED pin as output
  pinMode(LED, OUTPUT);
  
  //Start the I2C Bus as secondary on address 0x68
  Wire.begin(MY_ADDR); 
  
  //Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);

  sprintf(message, "Secondary is ready : Address 0x%x", MY_ADDR);
  Serial.println((char*)message);  
  Serial.println("Waiting for data from primary");  
}

void loop(void) {
  //Do nothing
}

void receiveEvent(int bytes) {
  while(Wire.available()) {
    rx_buffer[cnt++] = Wire.read();
  }
  
  rx_buffer[cnt] = '\0';
  cnt = 0;
  Serial.print("Received:");  
  Serial.println((char*)rx_buffer);  
}

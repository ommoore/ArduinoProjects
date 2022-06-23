/***********************************************
* @file  I2CSlaveTxString.ino
* @brief Arduino Mega2560 acting as I2C slave device to receive and
*        transfer a string, or collection of bytes, from master. 
*        Modified from example from: FastBit Embedded 
*        Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/
//Wire (I2C) Slave transmitter and receiver
//Arduino Mega2560 - [A4(SDA), A5(SCL)] or D20(SDA), D21(SCL)

#include <Wire.h>

#define SLAVE_ADDR 0x68

int LED = 13;
uint8_t active_command = 0xFF, led_status = 0;
char name_msg[32] = "This is a test message!\n";
uint16_t device_id = 0xFF45;

uint8_t get_len_of_data(void) {
  return (uint8_t)strlen(name_msg);
}

void setup() {
  //Define the LED pin as output
  pinMode(LED, OUTPUT);
  
  //Start the I2C bus as slave on address 9
  Wire.begin(SLAVE_ADDR); 
  
  //Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

//write
void receiveEvent(int bytes) {
  active_command = Wire.read();    // read one character from the I2C 
}

//read
void requestEvent() {
  if(active_command == 0x51) {
    uint8_t len = get_len_of_data();
    Wire.write(&len, 1);
    active_command = 0xff;
  }
  
  if(active_command == 0x52) {
    //Wire.write(strlen(name));
    Wire.write(name_msg, get_len_of_data());
    //Wire.write((uint8_t*)&name_msg[32], 18);
    active_command = 0xff;
  }
  //Wire.write("hello ");   //respond with message of 6 bytes as expected by master
}

void loop() { 
  //do nothing
}

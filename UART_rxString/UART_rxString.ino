/***********************************************
* @file  UART_rxString.ino
* @brief Arduino Mega2560 receiving a string, or 
*        collection of bytes, over UART Serial.
*        Modified example from: FastBit Embedded 
*        Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/                      

void setup() {
  Serial.begin(115200);
  
  //Define the LED pin as Output
  pinMode(13, OUTPUT);
  
  Serial.println("Arduino UART Receiver");
  Serial.println("-----------------------------");
}

void loop() {
  digitalWrite(13, LOW); 
  if(Serial.available()) {
    digitalWrite(13, HIGH); 
    
    //read data
    char in_read = Serial.read();
    
    //print data
    Serial.print(in_read);
  }
}

/***********************************************
* @file  UARTTxString.ino
* @brief Arduino Mega2560 receiving a string over 
*        UART Serial and retransmitting with toggled case. 
*        Modified from example from: FastBit Embedded 
*        Brain Academy (Udemy) course.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/      

void setup() {
  Serial.begin(115200);
  
  //Define the LED pin as Output
  pinMode(13, OUTPUT);
  
  Serial.println("Arduino Case Converter program running");
  Serial.println("-------------------------------------");  
}

char changeCase(char ch) {
  if((ch >= 'A') && (ch <= 'Z'))
    ch = ch + 32;
  else if((ch >= 'a') && (ch <= 'z'))
    ch = ch - 32;  
  return ch;
}

void loop() {
  digitalWrite(13, LOW); 
  
  //wait until data is received
  while(!Serial.available());
  digitalWrite(13, HIGH); 
  
  //read the data
  char in_read = Serial.read();
  
  //print the data
  Serial.print(changeCase(in_read));
}

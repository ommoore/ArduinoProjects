/***********************************************
* @file ImadaDST_Force_Gauge.ino
* @brief UART-RS232 interface between Arduino Mega2560 
*        and Imada DST Digital Force Sensor.
* @author Oliver Moore
* @version 1.0
***********************************************/
#include <SoftwareSerial.h>

//UART
#define rxPin 11 // receive data pin
#define txPin 12 // transmit data pin

// Command Macros -- refer to ASCII table
// Format: <UPPERCASE_ASCII_CHAR_0> ... <UPPERCASE_ASCII_CHAR_n> <CR>
#define TRANSMIT_DISPLAY_DATA   68 //D
#define RESET_TO_ZERO           90 //Z
#define MODE_REALTIME           84 //T
#define MODE_PEAK               80 //P
#define UNITS_IMPERIAL          79 //O  lbf/ozf
#define UNITS_METRIC            75 //K  kgf/gf
#define UNITS_SI                78 //N  Newtons
#define SET_HIGHLOW_SETPOINTS   69 //E HHHHLLLL
#define READ_HIGHLOW_SETPOINTS  70 //E **70 is an unused command but corrected for within switch statement**
#define POWER_OFF               81 //Q
#define CR                      13 //CR - carriage return

SoftwareSerial mySerial(rxPin, txPin); // D11 - RX, D12 - TX 
int lasersensor = A0;

void setup() {
  pinMode(rxPin, INPUT_PULLUP);
  pinMode(txPin, OUTPUT);
  pinMode(lasersensor, INPUT_PULLUP); // initialize laser sensor as INPUT_PULLUP

  Serial.begin(19200);   //Hardware (Serial)         - Console
  mySerial.begin(19200); //Software (SoftwareSerial) - UART-RS232
  while(!Serial); // wait for serial port to connect. Needed for native USB port only
}

void loop() {

  /* Executes once */
  mySerial.listen();
  if(mySerial.isListening()) {
    send_command(TRANSMIT_DISPLAY_DATA);
    //send_command(RESET_TO_ZERO);
    //send_command(MODE_REALTIME);
    //send_command(MODE_PEAK);
    //send_command(UNITS_IMPERIAL);
    //send_command(UNITS_METRIC);
    //send_command(UNITS_SI);
    //send_command(SET_HIGHLOW_SETPOINTS);
    //send_command(READ_HIGHLOW_SETPOINTS);
    //send_command(POWER_OFF);
  } else {
    Serial.println("Device is not listening");
  }
  /* Infinite Loop */
  while(1); 
}

void send_command(uint8_t command) {
  uint8_t cmd[] = {};
  int write_len = 0;

  switch(command) {
    case TRANSMIT_DISPLAY_DATA:
      Serial.println("TRANSMIT_DISPLAY_DATA");

      /* Command Byte(s) */
      cmd[write_len++] = command;
      
      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }
      
      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //[Indicator][Value][Unit][Mode][Judgement]
      //Indicator: + or -
      //Value: 4 digits with decimal point
      //Unit: Lbf, K=kgf, or N=Newtons
      //Mode: T=RealTime P=Peak
      //Judgement: L=Below Low Setpoint
      //           O=Between High and Low Setpoints
      //           H=Above High Setpoint
      //           E=Overload
      //Example: +123.4NTO
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;
      
    /**************************************/  
    case RESET_TO_ZERO:
      Serial.println("RESET_TO_ZERO");

      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;
      
    /**************************************/   
    case MODE_REALTIME:
      Serial.println("MODE_REALTIME");

      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/   
    case MODE_PEAK:
      Serial.println("MODE_PEAK");

      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }
      
      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/   
    case UNITS_IMPERIAL:
      Serial.println("UNITS_IMPERIAL");

      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/ 
    case UNITS_METRIC:
      Serial.println("UNITS_METRIC");

      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;
      
    /**************************************/   
    case UNITS_SI:
      Serial.println("UNITS_SI");
      
      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/ 
    case SET_HIGHLOW_SETPOINTS:
      Serial.println("SET_HIGHLOW_SETPOINTS");
      int H1,H2,H3,H4,L1,L2,L3,L4;

      /* Command Byte(s) */
      cmd[write_len++] = command;
      //EHHHHLLLL
      //High Setpoint: 0000-9999 default=capacity
      H1 = 9;
      H2 = 9;
      H3 = 9;
      H4 = 9;
      cmd[write_len++] = H1;
      cmd[write_len++] = H2;
      cmd[write_len++] = H3;
      cmd[write_len++] = H4;
      //Low Setpoint:  0000-9999 default=0
      L1 = 0;
      L2 = 0;
      L3 = 0;
      L4 = 0;
      cmd[write_len++] = L1;
      cmd[write_len++] = L2;
      cmd[write_len++] = L3;
      cmd[write_len++] = L4;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      //E[CR] Error
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/   
    case READ_HIGHLOW_SETPOINTS:  
      Serial.println("READ_HIGHLOW_SETPOINTS");

      /* Command Byte(s) */
      cmd[write_len++] = command - 1; //Adjusting command byte back to 'E' 

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //EHHHHLLL[CR]  **Ignore decimal point for high low setpoints
      //Setpoint values (4 digit integer)
      //HHHH=High, LLLL=Low
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/ 
    case POWER_OFF:
      Serial.println("POWER_OFF");
      
      /* Command Byte(s) */
      cmd[write_len++] = command;

      /* Append command with Carriage Return */
      cmd[write_len++] = CR;

      /* Write command bytes to DST Force Gauge */
      for(int i = 0; i < write_len; i++) {
          mySerial.write(cmd[i]);
      }

      /* Read response bytes from DST Force Gauge */
      /* Print Response to Serial Console */
      //R[CR] Executed
      while(mySerial.available() > 0) {
         Serial.print(mySerial.read());
      }
      Serial.println();
      break;

    /**************************************/   
    default:
      Serial.println("Invalid command");
      return;
  }
}

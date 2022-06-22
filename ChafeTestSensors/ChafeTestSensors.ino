/***********************************************
* @file  ChafeTestSensors.ino
* @brief Testbench sensor project for Arduino MEGA 2560
*        to obtain and store chafe test environmental 
*        data from the mass flow, differential pressure, 
*        humidity, proximity, and temperature sensors paired 
*        with RTC timestamps to SD Card via reader module.     
*        
* @author Oliver Moore
* @version 1.0
***********************************************/

/*  __Testing of multiple sensors with output printed to serial monitor and saved to SD card__
 *  Involved Hardware:
 *    . Arduino Mega2560
 *    . FS4000 Mass Flow Sensor via Half-Duplex (RS485) Modbus Master RTU configuration
 *    . MAX485/RS485 Transceiver between Arduino and FS4000 sensor using an additional GPIO pin
 *    . SM9236 Differential Pressure Sensor via I2C
 *    . DS3231 RTC (Real-Time Clock) Module via I2C
 *    . SD Card Reader via SPI
 *    . BME280 Humidity Sensor via SPI
 *    . E18-D80NK Proximity Sensor via GPIO 
 *    . DFRobot TS01 IR Temperature Sensor via ADC
 */

#include <ModbusMaster.h>
#include <SPI.h>
#include <SD.h>
#include <DS3231.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <stdint.h>
#include "Timer.h"

/********************** FS4000 Mass Flow Sensor MACROS ***********************/
/* Holding Register Addresses (Word Addressable: 16-bit -- 2 Bytes) */
#define Flowmeter_address           0x0001  //R/W  The address of flow meter in modbus protocol
#define FlowRate_address1           0x0002  // R   The current flow rate
#define FlowRate_address2           0x0003  // R   The current flow rate
#define BaudRate_address            0x0015  //R/W  The communication baud rate
#define GCF_address                 0x0016  //R/W  The gas correction factor
#define ResponseTime_address        0x0017  //R/W  The response time
#define FilterDepth_address         0x0018  //R/W  The filter depth
#define OffsetCalibration_address   0x0027  // W   Calibrate the offset
#define WriteProtect_address        0x0014  // W   Write protect for GCF, response time and filter depth. Single effective.

//Flowmeter Address Register Macros
#define FlowmeterAddress_Default    1

//Baud Rate Register Macros
#define BaudRate4800                0
#define BaudRate9600                1
#define BaudRate19200               2
#define BaudRate38400_Default       3    //default

//Gas Correction Factor Register Macros
#define GCF1000_Default             1000 //default
#define GCF500_Test                 500

//Response Time Register Macros
#define ResponseTime10ms_Default    10   //default
#define ResponseTime20ms            20
#define ResponseTime50ms            50
#define ResponseTime100ms           100
#define ResponseTime200ms           200
#define ResponseTime500ms           500
#define ResponseTime1000ms          1000

//Filter Depth Register Macros
#define FilterDepth0_Default        0
#define FilterDepth4_Test           4

//Offset Calibration Register Macro
#define OffsetCalibrationValue      0xAA55

//Write-Protect Register Macro
#define WriteProtectValue           0xAA55

/********************** MAX485/RS485 Transceiver MACROS ***********************/
//Transceiver Direction Pin (Both DE & RE are jumpered together)
#define MAX485_DE_RE                2

/********************** SM9236 Pressure Sensor MACROS ***********************/
/*  The memory address field sets the byte address of the first memory location to be 
 *  read from or written to. Only 16-bit-word aligned reads/writes are supported, 
 *  i.e. the LSB of memory address has to be zero always. The read/write data is 
 *  transferred MSB first, low byte before high byte.
 *  
 *  So the slaveAddress of 0x6D should actually be set to (0x6D << 1) or 0xDA
 *  (0x6D) : 0b 0110 1101 << 1  =  0b 1101 1010 : (0xDA)
*/
#define SM9236_slaveAddress         0x6C
#define SM9236_slaveAddressWrite    0xD8
#define SM9236_slaveAddressRead     0xD9
#define SM9236_commandRegister      0x22
#define SM9236_memoryAddressStart   0x2E
#define SM9236_temperatureRegister  SM9236_memoryAddressStart
#define SM9236_pressureRegister     0x30
#define SM9236_statusSyncRegister   0x32
#define SM9236_statusRegister       0x36

/* Status Bit Position MACROS (Same positioning for both Status & StatusSync registers) */
//Flags remain set until cleared by writing '1' to the respective bit position in STATUS register (not STATUS_SYNC).
//Writing 0xFFFF to the STATUS register will clear all event bits
#define idle_flag                   0   //0: chip busy, 1: chip idle
#define dsp_s_up_flag               3   //1: DSP_S reg has been updated. Cleared from Status when DSP_S is read and then copied to StatusSync
#define dsp_t_up_flag               4   //1: DSP_T reg has been updated. Cleared from Status when DSP_T is read and then copied to StatusSync
#define bs_fail_flag                7   //1: bridge supply failure occurred
#define bc_fail_flag                8   //1: sensor bridge check failure occurred
#define dsp_sat_flag                10  //1: a DSP computation leading to the current DSP_T or DSP_S values was saturated to prevent overflow
#define com_crc_error_flag          11  //1: communication CRC error
#define dsp_s_missed_flag           14  //1: dsp_s_up was 1 when DSP_S updated
#define dsp_t_missed_flag           15  //1: dsp_t_up was 1 when DSP_T updated

/**************** DS3231 RTC Module MACROS ****************/
#define RTC_Address                 0x68

/********************** SD Card Reader MACROS ***********************/
//SPI SD card pin definitions
/* MISO 50, MOSI 51, SCK 52, CS 4 */
#define SD_CS                       4

/********************** Adafruit BME280 Humidity Sensor MACROS ***********************/
/* MISO 50, MOSI 51, SCK 52, CS 53 */
#define BME_MISO                    50
#define BME_MOSI                    51 
#define BME_SCK                     52 
#define BME_CS                      53

#define SeaLevelPressureHPa         1013.25

/**************** E18-D80NK Proximity Sensor MACROS ****************/
#define PROXIMITY_SENSOR            3

/**************** GPIO MACROS ****************/
#define RASPBERRYPI                 5
#define LEAK_LED                    7

/* API Declarations for DS3231 RTC */
void initDS3231RTC(void);
void showTime_Date(void);

/* API Declarations for FS4000 Mass Flow Sensor */
void initFS4000Flowmeter(void);
void setStartupParameters(void);
void readStartupParameters(void);
void setFlowmeterAddress(uint16_t NewAddress);
void readFlowmeterAddress(void);
void readFlowRate(void);
void setBaudRate(uint16_t BaudRate);
void readBaudRate(void);
void setGasCorrectionFactor(uint16_t GasCorrectionFactor);
void readGasCorrectionFactor(void);
void setResponseTime(uint16_t ResponseTime);
void readResponseTime(void);
void setFilterDepth(uint16_t FilterDepth);
void readFilterDepth(void);
void offsetCalibration(void);
uint8_t writeProtection(void);

/* API Declarations for SM9236 Pressure Sensor */
void initSM9236PressureSensor(void);
void readSM9236PressureSensor(void);
void SM9236_clearSTATUSregister(void);
void SM9236_cmdRESET(void);

/* API Declarations for BME280 Humidity Sensor */
void initBME280HumiditySensor(void);
void readBME280HumiditySensor(void);

/* API Declarations for E18-D80NK Proximity IR Sensor */
void initE18D80NKProximitySensor(void);
void readE18D80NKProximitySensor(void);

/* API Declarations for DFRobot TS01 IR Temperature Sensor */
void readTS01IRTemperature(void);

void leakTime();
void aboutTime();

/* Object Declarations */
//instantiate ModbusMaster object for FS4000 mass flow sensor
ModbusMaster node;

//instantiate File object for SD Card reader
File dataFile;

//instantiate Real Time Clock objects for DS3231 RTC module
RTClib myRTC;
DS3231 Clock;
Timer t, t2;

//instantiate Hardware SPI BME object for BME280 humidity sensor
Adafruit_BME280 bme(BME_CS); // hardware SPI

//Interrupt Counter for Proximity Sensor
volatile unsigned long IR_cycle_counter = 693150;
volatile unsigned int leak_count = 0;

int leak_flag = 0;
int enough_time = 0;
int leak_confirmed = 0;

/* Calibration Enable/Disable */
//Set to 1 to enable Calibration tool
int Calibration = 0;

/* Callback Functions required for Modbus Master communication */
//Callback function declared in <ModbusMaster.h>
void preTransmission() {
  //Pull both DE/RE pins high to transmit data
  digitalWrite(MAX485_DE_RE, 1);
}

//Callback function declared in <ModbusMaster.h>
void postTransmission() {
  //Pull both DE/RE pins high to receive data
  digitalWrite(MAX485_DE_RE, 0);
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                          SETUP
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void setup() {
  //Change the first number to adjust the time in milliseconds
  int afterEvent = t.after(120000, aboutTime, (void*)60); //2 minutes

  leak_flag = 0;
  enough_time = 0;
  leak_confirmed = 0;
  
  pinMode(RASPBERRYPI, OUTPUT);
  digitalWrite(RASPBERRYPI, HIGH);

  pinMode(LEAK_LED, OUTPUT);
  digitalWrite(LEAK_LED, LOW);
  
  /* Serial Setup (Port 0) */
  //Open Serial communication for serial monitor usage and wait for port to open:
  Serial.begin(9600);
  while(!Serial);
  //Serial.println("Hello World");

  /* Serial1 Setup (Port 1) */
  //Open Serial1 communication for ModBus Master usage
  Serial1.begin(38400);

  //Initialize the I2C bus
  Wire.begin();

  /* DS3231 RTC Init */
  initDS3231RTC();

  /* FS4000 Mass Flow Sensor Init */
  initFS4000Flowmeter();
  
  /* SD Card Init */
  pinMode(SD_CS, OUTPUT);
  if(!SD.begin(SD_CS)) {
    Serial.println("SD card failed or not present");
    while(1);
  }
  
  /* BME280 Humidity Sensor Init */
  initBME280HumiditySensor();

  /* SM9236 Pressure Sensor Init */
  initSM9236PressureSensor();  

  /* E18-D80NK Proximity IR Sensor Init */
  initE18D80NKProximitySensor();

  /* Testing FS4000 Write APIs */
  //setStartupParametersFS4000();

  /* Testing FS4000 Read APIs */
  //readStartupParametersFS4000();
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                          LOOP
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void loop() {
  if(Calibration) {
    /*** Calibration of FS4000 ***/
    offsetCalibration();
    while(1); //hang after calibration
  
  } else {
    /*** Application ***/
    /*                      Example Output
    *  03/25/2021 -- 18:11:01 FS4000: 0.00SCFM | SM9236: 66.67%fs 400Pa | BME280: 70*C 5hPA 60% 10m | Cycle#: 1
    */
    dataFile = SD.open("Test1.txt", FILE_WRITE); //Keep this filename very short ex. Test1.txt
    if(dataFile) {
      showTime_Date();
      readFlowRate();                 //comment out to disable sensor for testing
      t.update();                     //Startup Timer check
      readSM9236PressureSensor();     //comment out to disable sensor for testing
      readBME280HumiditySensor();     //comment out to disable sensor for testing
      readE18D80NKProximitySensor();  //comment out to disable sensor for testing
      //readTS01IRTemperature();      //comment out to disable sensor for testing
      
      Serial.println();           //Newline
      dataFile.println();         //Newline
      dataFile.close();
    } else {
      Serial.println("Error Opening T1.txt");
    }
  }
  delay(1000);
}

/****************** API Definitions for DS3231 RTC Sensor ******************/
/********************** RTC Init **********************/
void initDS3231RTC(void) {
  Wire.beginTransmission(RTC_Address);
  byte busStatus = Wire.endTransmission();
  if(busStatus != 0x00) {
    Serial.println("I2C Bus Error/DS3231 not Found!");
    while(1); //wait for ever
  }
  
  // The following lines can be uncommented to set the date and time manually
  // 3/25/2021
  // Clock.setMonth((byte) 6);
  // Clock.setDate((byte) 28);
  // Clock.setYear((byte) 21);

  // 4:36:00 PM
  // Clock.setHour((byte) 15);  //24 hour format
  // Clock.setMinute((byte) 39);
  // Clock.setSecond((byte) 00);

}

void showTime_Date(void) {
  DateTime now = myRTC.now();

  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" -- ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" ");

  dataFile.print(now.month(), DEC);
  dataFile.print('/');
  dataFile.print(now.day(), DEC);
  dataFile.print('/');
  dataFile.print(now.year(), DEC);
  dataFile.print(" -- ");
  dataFile.print(now.hour(), DEC);
  dataFile.print(":");
  dataFile.print(now.minute(), DEC);
  dataFile.print(":");
  dataFile.print(now.second(), DEC);
  dataFile.print(" ");
}

/****************** API Definitions for FS4000 Mass Flow Sensor ******************/
/********************** FLOWMETER Init **********************/
void initFS4000Flowmeter(void) {
  /*** Modbus Master Setup ***/
  //Communicate with Modbus slave ID 1 over Serial1 (port 1)
  node.begin(1, Serial1);
  
  //Transceiver Direction Pin
  pinMode(MAX485_DE_RE, OUTPUT);
 
  //Initialize DirPin in receive mode
  digitalWrite(MAX485_DE_RE, 0);

  //Callbacks allow for proper configuration of RS485 transceiver DE and RE pins for Read/Write transactions
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void setStartupParametersFS4000(void){
  /* Configure Device Parameters using APIs with provided Macros */
  //Set FS4000 Device Address using provided macros
  setFlowmeterAddress(FlowmeterAddress_Default);
  
  //Set Baud Rate of FS4000 using provided macros
  setBaudRate(BaudRate38400_Default);

  //Set Gas Correction Factor of FS4000 using provided macros
  setGasCorrectionFactor(GCF1000_Default);

  //Set Response Time of FS4000 using provided macros
  setResponseTime(ResponseTime10ms_Default);

  //Set Filter Depth of FS4000 using provided macros
  setFilterDepth(FilterDepth0_Default);
}

void readStartupParametersFS4000(void){
  readFlowmeterAddress();
  readBaudRate();
  readGasCorrectionFactor();
  readResponseTime();
  readFilterDepth();
}

/********************** FLOWMETER ADDRESS **********************/
void setFlowmeterAddress(uint16_t NewAddress) {
  //Note: Default value is 1. 0 is broadcast address
  //Value from 1 to 255 except 157 (0x9d)
  uint8_t result;

  if((NewAddress != 155) && (NewAddress >= 0) && (NewAddress < 256)) {
    //Valid Flowmeter Address
    if(NewAddress == 0) {
      //0 : broadcast address
      //do nothing 
    } else {
      //1-156 or 158-255: slave addresses
      //write new Flowmeter address
      result = node.writeSingleRegister(Flowmeter_address, NewAddress);
      if(!result) {
        Serial.print("New Flowmeter address: ");
        Serial.println(NewAddress);        
      } else {
        Serial.print("Error: ");
        Serial.println(result);
      }
    }
  } else {
    //Invalid Flowmeter Address
    Serial.println("Invalid Flowmeter Address value. Accepted values: 0-255 except 157 (0x9d)");
  }
}

void readFlowmeterAddress(void) {
  //Note: Default value is 1. 0 is broadcast address
  uint8_t result;
  uint16_t data;

  //Read Flowmeter Address
  result = node.readHoldingRegisters(Flowmeter_address, 1);
  if(result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Flowmeter Address: ");
    Serial.println(data);  
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

/********************** FLOWMETER FLOW RATE **********************/
void readFlowRate(void) {
  uint8_t result;
  uint16_t data[2];
  float Flowrate;

  // slave: read (2) 16-bit registers starting at register 2 (i.e 2&3) to RX buffer
  result = node.readHoldingRegisters(FlowRate_address1, 2);
  if(result == node.ku8MBSuccess) {
    for(int i = 0; i < 2; i++) {
      data[i] = node.getResponseBuffer(i);
    }
    
    //Calculation
    // SLPM: Standard Liters per minute
    // SCFM: Standard Cubic Feet per minute
    Flowrate = float(((((data[0] * 65536) + data[1]) / 1000) / 28.317)); // Converting from SLPM to SCFM 
    
    //Print Flowrate to console
    Serial.print("FS4000 Flowrate: ");
    Serial.print(Flowrate);
    Serial.print("SCFM");
    Serial.print(" | ");
    
    //Write Flowrate to dataFile
    dataFile.print("FS4000 Flowrate: ");
    dataFile.print(Flowrate);
    dataFile.print("SCFM");
    dataFile.print(" | ");    
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

/********************** FLOWMETER BAUD RATE **********************/
void setBaudRate(uint16_t BaudRate) {
  //Note: Default value is 3
  uint8_t result;

  if((BaudRate == 0) || (BaudRate == 1) || 
     (BaudRate == 2) || (BaudRate == 3)) {
    //Valid Baud Rate value
    //Write new Baud Rate
    result = node.writeSingleRegister(BaudRate_address, BaudRate);
    if(!result) {
      Serial.print("New Baud Rate: ");
      Serial.println(BaudRate);   
    } else {
      Serial.print("Error: ");
      Serial.println(result);
    }
  } else {
    //Invalid Baud Rate value
    Serial.println("Invalid Baud Rate value. Accepted values: 0 (4800), 1 (9600), 2 (19200), 3 (38400)");
  }
}

void readBaudRate(void) {
  //Note: Default value is 3
  uint8_t result;
  uint16_t data;
  
  //Read Baud Rate
  result = node.readHoldingRegisters(BaudRate_address, 1);
  if(result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Baud Rate: ");
    Serial.println(data);
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

/********************** FLOWMETER Gas Correction Factor **********************/
void setGasCorrectionFactor(uint16_t GasCorrectionFactor) {
  //Note: Default value is 1000 == 1.0 GCF (air)
  //Note: Need to set write-protect
  
  uint8_t result, wp_result;

  //Write new Gas Correction Factor
  result = node.writeSingleRegister(GCF_address, GasCorrectionFactor);
  if(!result) { 
    //Write-Protect Check
    wp_result = writeProtection();
    if(!wp_result) {
      Serial.print("New Gas Correction Factor (GCF): ");
      Serial.println(GasCorrectionFactor);
    }
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

void readGasCorrectionFactor(void) {
  //Note: Default value is 1000 == 1.0 GCF (air)
  uint8_t result;
  uint16_t data;
  
  //Read Gas Correction Factor
  result = node.readHoldingRegisters(GCF_address, 1);
  if(result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Gas Correction Factor (GCF): ");
    Serial.println(data);  
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

/********************** FLOWMETER Response Time **********************/
void setResponseTime(uint16_t ResponseTime) {
  //Note: Default value is 10
  //Note: Need to set write-protect
  uint8_t result, wp_result;

  if((ResponseTime ==  10) || (ResponseTime ==  20) || 
     (ResponseTime ==  50) || (ResponseTime == 100) || 
     (ResponseTime == 200) || (ResponseTime == 500) || (ResponseTime == 1000)) {
    //Valid Response Time value
    //Write new Response Time
    result = node.writeSingleRegister(ResponseTime_address, ResponseTime);
    if(!result) { 
      //Write-Protect Check
      wp_result = writeProtection();
      if(!wp_result) {
        Serial.print("New Response Time: ");
        Serial.println(ResponseTime);
      } 
    } else {
      Serial.print("Error: ");
      Serial.println(result);
    }   
  } else {
    //Invalid Response Time value
    Serial.println("Invalid Response Time value. Accepted values (ms): 10,20,50,100,200,500,1000");
  }
}

void readResponseTime(void) {
  //Note: Default value is 10
  uint8_t result;
  uint16_t data;

  //Read Response Time
  result = node.readHoldingRegisters(ResponseTime_address, 1);
  if(result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Response Time: ");
    Serial.println(data); 
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

/********************** FLOWMETER Filter Depth **********************/
void setFilterDepth(uint16_t FilterDepth) {
  //Note: Default value is 0
  //Values: 0 and 4~255
  //Note: Need to set write-protect
  uint8_t result, wp_result;

  if((FilterDepth == 0) || ((FilterDepth > 3) && (FilterDepth < 256))) {
    //Valid Filter Depth value
    //Write new Filter Depth
    result = node.writeSingleRegister(FilterDepth_address, FilterDepth);
    if(!result) { 
      //Write-Protect Check
      wp_result = writeProtection();
      if(!wp_result) {
        Serial.print("New Filter Depth: ");
        Serial.println(FilterDepth);
      }  
    } else {
      Serial.print("Error: ");
      Serial.println(result);
    }  
  } else {
    //Invalid Filter Depth value
    Serial.println("Invalid Filter Depth value. Accepted values: 0 and 4~255");
  }
}

void readFilterDepth(void) {
  //Note: Default value is 0
  //Values: 0 and 4~255
  uint8_t result;
  uint16_t data;

  //Read Filter Depth
  result = node.readHoldingRegisters(FilterDepth_address, 1);
  if(result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Filter Depth: ");
    Serial.println(data);  
  } else {
    Serial.print("Error: ");
    Serial.println(result);
  }
}

/********************** FLOWMETER Offset Calibration **********************/
void offsetCalibration(void) {
  //Note: When using, ensure there is NO flow in the pipeline
  char answer = 0;
  uint8_t result;

  //Prompt User for Input
  Serial.println("Are you sure you want to perform Offset Calibration? Enter 1 for (YES) or AnythingElse for (NO)"); 
  Serial.println("Ensure there is NO flow in the pipeline!!");
  
  while(Serial.available() == 0); //Wait for user input
  
  answer = Serial.read(); //Read the user input
  Serial.println(answer);
  
  if(answer == '1') {
    Serial.println("Starting calibration..");
    result = node.writeSingleRegister(OffsetCalibration_address, OffsetCalibrationValue);
    if(!result) {
       Serial.println("Calibration complete.");
    } else {
       Serial.print("Error: ");
       Serial.println(result);
    }  
  } else {
    Serial.println("Exiting Offset Calibration tool..");
  }
}

/********************** FLOWMETER Write Protection **********************/
uint8_t writeProtection(void) {
  uint8_t wp_result;

  wp_result = node.writeSingleRegister(WriteProtect_address, WriteProtectValue);
  if(!wp_result) {
    //Write Protection success
    //Serial.println("Write Protection Successful");
  } else {
    //Write Protection failure
    Serial.println("Failed to set Write Protection");
    Serial.print("Exception Number: ");
    Serial.println(wp_result);
  }
 
  return wp_result;
}

/****************** API Definitions for SM9236 Pressure Sensor ******************/
/********************** PRESSURE Init **********************/
void initSM9236PressureSensor(void) {
  
  uint8_t status_low, status_high;
  int retry = 1;
  int pressureCheck = 0;
  int count = 0;

  //Perform reset power-up sequence
  //Registers are loaded with data from the configuration memory, and CRC check is performed
  //SM9236_cmdRESET();
  
  
  //Check STATUS.dsp_s_up and STATUS.dsp_t_up (bits 3 and 4 respectively)
  //After power up, it is necessary to wait until these two flags have been set at least once
  //before using the data. It is not sufficient to wait just for a fixed time delay
  //***Ignoring Temperature data and checks***
  while(retry) {
    pressureCheck = 0;
    count++;
    
    /* Unprotected Read of 1 16-bit Word (2 bytes) starting from memory address 0x36 */
    //Master Requesting from Slave
    Wire.beginTransmission(SM9236_slaveAddress);         //0x6C -> 0xD8 (beginTransmission() sets R/W bit to write)
    Wire.write(SM9236_statusRegister);                   //0x36
    Wire.endTransmission(false);                         //restart condition (repeated start)
    
    //Slave Replying to Master
    Wire.requestFrom(SM9236_slaveAddress,2);             //0x6C -> 0xD9 (requestFrom sets R/W bit to read)
    
    if(Wire.available() <= 2) {
      status_low = Wire.read();     //Lo-Byte ad. 0x36
      status_high = Wire.read();    //Hi-Byte
    }
         
    //Serial.print(status_low, HEX);
    //Serial.print(" ");
    //Serial.println(status_high, HEX);
    
    //Status Register check (not Status SYNC Register)
    if(status_low & (1 << dsp_s_up_flag)) {         //Check if bit3 dsp_s_up is set
      //Serial.println("Pressure reading is valid!");
      pressureCheck = 1;
    }
    
    if(status_high & (1 << com_crc_error_flag)) {   //check if bit11 com_crc_error is set
      Serial.println("NVM CRC error, temp and pressure readings will never be updated.");
      SM9236_clearSTATUSregister();
      retry = 1;
    } else if(pressureCheck){
      //Serial.println("SM9236 Init Success!");
      retry = 0; //exit loop
    } else {
      //No CRC error, but temperature or pressure readings (or both) are invalid then retry
      SM9236_clearSTATUSregister();
      retry = 1;
    }
  }
}

/********************** PRESSURE Read **********************/
void readSM9236PressureSensor(void) {

  uint8_t syncStatus0, syncStatus1;
  uint8_t dsp_t0, dsp_t1, dsp_s0, dsp_s1; //signed values stored as 2's complement
  int SensorValue = 0;
  int16_t dsp_s_word;
  int16_t p_count;
  float p_unit;

  int retry = 1;
  
  while(retry) {
    /* Unprotected Read of 3 16-bit Words (6 bytes) starting from memory address 0x2E */
    //Master Requesting from Slave
    Wire.beginTransmission(SM9236_slaveAddress);         //0x6C -> 0xD8 (beginTransmission() sets R/W bit to write)
    Wire.write(SM9236_memoryAddressStart);               //0x2E
    Wire.endTransmission(false);                         //restart condition

    delay(50); //step response settling time of pressure reading is typically 45ms
    
    //Slave Replying to Master
    Wire.requestFrom(SM9236_slaveAddress,6);         //0x6C -> 0xD9 (requestFrom sets R/W bit to read)

    if(Wire.available() <= 6) {
      dsp_t0 = Wire.read();          //Lo-Byte ad. 0x2E               -- Sensor sends 0xF2
      dsp_t1 = Wire.read();          //Hi-Byte                        -- Sensor sends 0x7D
      dsp_s0 = Wire.read();          //Lo-Byte ad. 0x30               -- Sensor sends 0xEA
      dsp_s1 = Wire.read();          //Hi-Byte                        -- Sensor sends 0x82
      syncStatus0 = Wire.read();     //Sync'ed Status (b7-b0) ad.0x32 -- Sensor sends 0x1E
      syncStatus1 = Wire.read();     //Sync'ed Status (b15-b8)        -- Sensor sends 0x00
    }
    
    /* Debug statements to compare values */
    //Serial.println(dsp_s0, HEX);
    //Serial.println(dsp_s1, HEX);
    //Serial.println(syncStatus0, HEX);
    //Serial.println(syncStatus1, HEX);
    
    /* Error Checking */
    //The synchronized status STATUS_SYNC register can be used to continuously poll the pressure, temperature and status of the device with
    //a single read command by reading three 16 bit words starting at address 0x2E. By evaluating STATUS_SYNC.dsp_t_up and
    //STATUS_SYNC.dsp_s_up it can be determined if the values in DSP_T and DSP_S acquired during the same read contain recently updated
    //temperature or pressure values.

    //Status Sync Register check (not Status Register)
    if(syncStatus0 & (1 << dsp_s_up_flag)) {         //Check if bit3 dsp_s_up is set
      //Data is in Sync!
      retry = 0;
    } else {
      Serial.println("Pressure reading is not in sync!");
      SM9236_clearSTATUSregister();
    } 
  }

  /* Data Calculations taken from page 9 of datasheet */
  dsp_s_word = ((dsp_s1 << 8) | dsp_s0);                   //Combining 2 bytes into 16-bit word
  p_count = (int16_t)dsp_s_word;                           //Signed 16-bit value pressure reading
  p_unit = (((p_count + 26215.00) / 52429.00) * 600.00);    //Calibrated Floating Point Pressure Units 0-600 Pascal

  /* Debug statements to compare values */
  //Serial.println(dsp_s0, HEX);
  //Serial.println(dsp_s1, HEX);
  //Serial.println(dsp_s_word, HEX);
  //Serial.println(p_count, DEC);
  //Serial.println(p_unit, 2);
    
  Serial.print("SM9236: ");
  Serial.print(p_unit);
  Serial.print("Pa");
  Serial.print(" | ");

  dataFile.print("SM9236: ");
  dataFile.print(p_unit);
  dataFile.print("Pa");
  dataFile.print(" | ");
  
  //If pressure reading is ever above 350Pa
  if(p_unit > 350) {
    //reset leak counter back to zero.
    leak_count = 0;
  }
  
  //If pressure drops below 350Pa AND leak has not already been confirmed AND enough time has elapsed after reset...
  if((p_unit < 350) && (leak_flag == 0) && (enough_time == 1)) {

    //increment leak counter
    leak_count++;

    //if 100 consecutive readings under 350Pa were detected, then assume leak is confirmed
    if(leak_count >= 100) {
      leak_flag = 1;
    }
    
    //Once leak has been confirmed...
    //notify serial monitor, turn on LED, and notify Raspberry Pi's PiCamera to start recording
    if(leak_flag) {
      Serial.print("Leak Confirmed");
      //Turn on LED to signal when leak occured for PiCamera video feed
      digitalWrite(LEAK_LED, HIGH);
      //Notify RaspberryPi that leak has occured
      digitalWrite(RASPBERRYPI, LOW);
    }
  }
}

/********************** PRESSURE Clear Status Flags **********************/
void SM9236_clearSTATUSregister(void) {
  //Write of one 16-bit word (contents 0xCF9E) to 
  //address 0x36 to clear events in STATUS register
  Wire.beginTransmission(SM9236_slaveAddress);         //0x6C -> 0xD8 (beginTransmission() sets R/W bit to write)
  Wire.write(SM9236_statusRegister);                   //memory address of Status Register                          
  Wire.write(0x9E);                                    //STATUS Lo-Byte Ad. 0x36   LSB First
  Wire.write(0xCF);                                    //STATUS Hi-Byte            MSB Last
  Wire.endTransmission();                              //stop condition
}

/********************** PRESSURE Reset Command **********************/
void SM9236_cmdRESET(void) {
  //Write of one 16-bit word (contents 0xB169) to 
  //address 0x22 to perform reset sequence of sensor
  Wire.beginTransmission(SM9236_slaveAddress);         //0x6C -> 0xD8 (beginTransmission() sets R/W bit to write)  
  Wire.write(SM9236_commandRegister);                  //memory address of CMD register                          
  Wire.write(0x69);                                    //CMD[7:0]  Lo-Byte Ad. 0x22   LSB First
  Wire.write(0xB1);                                    //CMD[15:8] Hi-Byte            MSB Last
  Wire.endTransmission(true);                          //stop condition
  delay(50); //18ms typical power-up time
}

/****************** API Definitions for BME280 Humidity Sensor ******************/
/********************** HUMIDITY Init **********************/
void initBME280HumiditySensor(void) {
  unsigned status;    
  status = bme.begin();  
  if(!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      while(1);
  }
}

/********************** HUMIDITY Read **********************/
void readBME280HumiditySensor(void) {
  Serial.print("BME280: "); 
  Serial.print(bme.readTemperature());
  Serial.print("C ");
  Serial.print(bme.readPressure() / 100.0);
  Serial.print("hPa ");
  Serial.print(bme.readHumidity());
  Serial.print("% ");
  //Serial.print(bme.readAltitude(SeaLevelPressureHPa));
  //Serial.print("m");
  Serial.print(" | ");

  dataFile.print("BME280: "); 
  dataFile.print(bme.readTemperature());
  dataFile.print("*C "); 
  dataFile.print(bme.readPressure() / 100.0);
  dataFile.print("hPa ");
  dataFile.print(bme.readHumidity());
  dataFile.print("% ");
  dataFile.print(bme.readAltitude(SeaLevelPressureHPa));
  dataFile.print("m");
  dataFile.print(" | ");
}

/****************** API Definitions for E18-D80NK Proximity Sensor ******************/
/********************** PROXIMITY Init **********************/
void initE18D80NKProximitySensor(void){
  pinMode(PROXIMITY_SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_SENSOR), proximityISR, FALLING);
}

void proximityISR(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  
  //If interrupts come faster than 200ms, assume it's a bounce and ignore
  if(interrupt_time - last_interrupt_time > 200) {
    IR_cycle_counter++;
    last_interrupt_time = interrupt_time;
  }
}

void readE18D80NKProximitySensor(void) {
  noInterrupts(); //disable interrupts while reading 16-bit counter (MCU requires 2 steps to read, 8-bit at a time)
  unsigned long result = IR_cycle_counter;
  interrupts();   //enable interrupts
  
  Serial.print("Cycle: ");
  Serial.print(result);
  dataFile.print("Cycle: ");
  dataFile.print(result);
}

/****************** API Definitions for DFRobot TS01 IR Temperature Sensor ******************/
void readTS01IRTemperature(void) {
  unsigned int ADC_Value;
  float i, j;
  
  ADC_Value = analogRead(A3);
  i = (((double)ADC_Value) / (204.8*0.12));
  j = (9*((i-4)/16*450-70)) / 5 + 32;
  Serial.print(" | ");
  Serial.print(i);
  Serial.print(" mA, "); //printed current value
  Serial.print(j);
  Serial.print(" F");    //printed temperature value
}

/****************** Timer Callback ******************/
void aboutTime() {
  //Serial.println("Startup timer has finished");
  enough_time = 1;
}

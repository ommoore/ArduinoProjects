# ArduinoProjects

## ChafeTestSensors
  **Description:** \
  Sensor project for Arduino MEGA 2560 to obtain and store chafe test environmental data from the mass flow, differential pressure,
  humidity, proximity, and temperature sensors paired with RTC timestamps to SD Card via reader module.
  
  **Hardware:**
  - Arduino Mega2560
  - FS4000 Mass Flow Sensor via Half-Duplex (RS485) Modbus Master RTU configuration 
  - MAX485/RS485 Transceiver between Arduino and FS4000 sensor using an additional GPIO pin
  - SM9236 Differential Pressure Sensor via I2C
  - DS3231 RTC (Real-Time Clock) Module via I2C
  - SD Card Reader via SPI
  - BME280 Humidity Sensor via SPI
  - E18-D80NK Proximity Sensor via GPIO
  - DFRobot TS01 IR Temperature Sensor via ADC 
    
## ChafeTestTimers
  **Description:** \
  Testbench automation project for Arduino MEGA 2560 using timer interrupts to control speed & direction of stepper motor, as well as pump
  operation, with additional hardware interrupts configured to IR switches used for boundary detection.

  **Hardware:**
  - Arduino Mega2560
  - 2HSS60 Hybrid Stepper Servo Drive + Stepper motor
  - (x2) HiLetgo TCRT5000 IR photoelectric switch
  - Breakout board with L293D Motor Driver IC + pump

## I2CPrimary_rxString
 **Description:** \
 Arduino Mega2560 acting as I2C primary device to receive a string, or collection of bytes, from secondary. 
 Modified example from: FastBit Embedded Brain Academy (Udemy) course.

## I2CPrimary_rxStringLen
 **Description:** \
 Arduino Mega2560 acting as I2C primary device to receive a variable-length string from secondary. 
 Modified example from: FastBit Embedded Brain Academy (Udemy) course.
 
## I2CSecondary_rxString
 **Description:** \
 Arduino Mega2560 acting as I2C secondary device to receive a string, or collection of bytes, from primary. 
 Modified example from: FastBit Embedded Brain Academy (Udemy) course.
 
## I2CSecondary_txString
 **Description:** \
 Arduino Mega2560 acting as I2C secondary device to receive and transfer a string from primary. 
 Modified example from: FastBit Embedded Brain Academy (Udemy) course.

## ImadaDST_Force_Gauge
  **Description:** \
  UART-RS232 interface between Arduino Mega2560 and Imada DST Digital Force Sensor.

## SDP8xx-DiffPressure_BME280-Humidity
  **Description:** \
  LCD Display of SDP8xx Differential Pressure Sensor and BME280 Humdidity Sensor data.

## SmartWateringSystem
  **Description:** \
  Elecrow smart watering system for 1-4 plants using a system of 4 capacitive moisture sensors, 4 relays and one pump. LCD functionality for 
  monitoring real-time moisture readings. Modification of original source code.
  
## SPISecondary_cmdHandling
  **Description:** \
  Arduino Mega2560 acting as SPI secondary device to receive a command byte and respond accordingly. 
  Modified from example from: FastBit Embedded Brain Academy (Udemy) course.

## SPISecondary_rxString
  **Description:** \
  Arduino Mega2560 acting as SPI secondary device to receive a string, or collection of bytes, from primary. 
  Modified example from: FastBit Embedded Brain Academy (Udemy) course.

## SPISecondary_UARTrxSPItx
  **Description:** \
  Arduino Mega2560 acting as SPI secondary device to read Serial UART data into buffer and retransmit over SPI. 
  Modified example from: FastBit Embedded Brain Academy (Udemy) course.

## UART_rxString
  **Description:** \
  Arduino Mega2560 receiving a string, or collection of bytes, over UART Serial.
  Modified example from: FastBit Embedded Brain Academy (Udemy) course.
  
## UART_txString
  **Description:** \
  Arduino Mega2560 receiving a string over UART Serial and retransmitting with toggled case. 
  Modified example from: FastBit Embedded Brain Academy (Udemy) course.

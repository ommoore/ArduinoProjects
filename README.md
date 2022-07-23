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

## EddyCurrentBraking
  **Description:** \
  Arduino automation of an eddy-current braking system consisting of two Unos, an aluminum 6061 disc rotated radially on a single axis by a 
  stepper motor driven timing belt system, a neodymium permanent magnet circular Halbach array for braking, and multiple sensors with data to 
  be displayed to an LCD. The originally proposed subsystem using a relay-controlled electromagnet failed to produce a suitable braking EMF due 
  to the magnetic field line orientation. The alternate solution uses a circular Halbach array which produces field lines that penetrate the 
  aluminum disc when manually holding it parallel to the surface, causing a braking force. An improved design would make use of a relay-controlled 
  electromagnet with a better field line orientation. Sensor data output includes: stepper motor voltage & current (optional), RPM, and the 
  localized temperature change of aluminum 6061 disc during braking. Contained are two applications to be run concurrently on two Arduino Unos. 
  One application handles the retrieval and formatting of sensor data to be displayed on the 16x2 LCD. Consider adding an OLED LCD for displaying 
  more data. The other application handles the stepper motor parameter configuration and operation. Configured the stepper motor to act as a 
  conventional DC motor by setting a constant RPM and high microstepping value in order to reduce oscillations in the system. A greater microstepping 
  value will yield higher RPM but reduce torque.      
  
  **Hardware:**
  - SparkFun RedBoard Arduino Uno Rev3
  - Arduino Uno Rev3
  - Wantai 42BYGHW811 Bipolar Stepper Motor - 2.5A, 1.8°/step, 1/16 Microstep Setting
  - SparkFun L6470 AutoDriver Stepper Motor Driver (SPI)
  - AttoPilot Voltage and Current Sense Breakout - 50V/90A (Optional)
  - APW Company Inc APW EM100-12-222 Electromagnet (Original Design) **Electromagnet**
  - KG4 Controls KG4-SSR-V1.0 Solid State Relay (Original Design) **Electromagnet**
  - Melexis US1881 Hall-Effect Latch 
  - Melexis MLX90614 IR Temperature Sensor (I2C)
  - SparkFun UART Serial Enabled 16x2 White on Black 3.3V LCD

  **Materials:**
  - Aluminum 6061 Disc - 9" Diameter 0.25" Thickness 
  - Steel Rod - 3' Length 3/8" Diameter
  - (x2) Radial Ball Bearings - 3/8" Inner Diameter
  - Timing Belt Pulley System
	 - 5mm 60-Tooth Wheel
	 - 5mm 20-Tooth Wheel
	 - 200mm x 6mm Timing Belt
  - ArxPax Circular Halbach Array (Alternate Solution) **Neodymium Permanent Magnet**
  - Small Neodymium Magnet attached to disc to pulse Hall-Effect Latch for RPM measurement
  - Plywood for display stand
  - Magnetic Viewing Film to view field lines
  - Breadboards

**Directory Contents:**
```
**EddyCurrentBraking**
|
└─── EddyCurrentBraking_SensorsLCD.ino
└─── EddyCurrentBraking_StepperMotor.ino
```

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
  16x2 LCD display of SDP8xx Differential Pressure Sensor and BME280 Humidity Sensor data.

## SmartWateringSystem
  **Description:** \
  Elecrow smart watering system for 1-4 plants using a system of 4 capacitive moisture sensors, a 4-channel relay and one pump. OLED LCD used to 
  display real-time soil moisture readings. Modification of original source code.
  
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

/***********************************************
* @file  EddyCurrentBraking_SensorsLCD.ino
* @brief Arduino automation of an eddy-current braking system 
*        consisting of two Unos, an aluminum 6061 disc rotated 
*        radially on a single axis by a stepper motor driven 
*        timing belt system, a neodymium permanent magnet circular 
*        Halbach array for braking, and multiple sensors with data 
*        to be displayed to an LCD. The originally proposed subsystem 
*        using a relay-controlled electromagnet failed to produce a 
*        suitable braking EMF due to the magnetic field line orientation. 
*        The alternate solution uses a circular Halbach array which 
*        produces field lines that penetrate the aluminum disc when 
*        manually holding it parallel to the surface, causing a braking 
*        force. An improved design would make use of a relay-controlled 
*        electromagnet with a better field line orientation. Sensor data 
*        output includes: stepper motor voltage & current (optional), RPM, 
*        and the localized temperature change of aluminum 6061 disc during 
*        braking. This application handles the retrieval and formatting of 
*        sensor data to be displayed on the 16x2 LCD. Consider adding an 
*        OLED LCD for displaying more data.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/

/*  Directory Contents:
 *    EddyCurrentBraking
 *    └─── EddyCurrentBraking_SensorsLCD.ino
 *    └─── EddyCurrentBraking_StepperMotor.ino
 *    
 *  Hardware:
 *    . SparkFun RedBoard Arduino Uno Rev3
 *    . Arduino Uno Rev3
 *    . Wantai 42BYGHW811 Bipolar Stepper Motor - 2.5A, 1.8°/step, 1/16 Microstep Setting
 *    . SparkFun L6470 AutoDriver Stepper Motor Driver (SPI)
 *    . AttoPilot Voltage and Current Sense Breakout - 50V/90A (Optional)
 *    . APW Company Inc APW EM100-12-222 Electromagnet (Original Design) **Electromagnet**
 *    . KG4 Controls KG4-SSR-V1.0 Solid State Relay (Original Design) **Electromagnet**
 *    . Melexis US1881 Hall-Effect Latch 
 *    . Melexis MLX90614 IR Temperature Sensor (I2C)
 *    . SparkFun UART Serial Enabled 16x2 White on Black 3.3V LCD
 *    
 *  Materials:
 *    . Aluminum 6061 Disc - 9" Diameter 0.25" Thickness 
 *    . Steel Rod - 3' Length 3/8" Diameter
 *    . (x2) Radial Ball Bearings - 3/8" Inner Diameter
 *    . Timing Belt Pulley System
 *       - 5mm 60-Tooth Wheel
 *       - 5mm 20-Tooth Wheel
 *       - 200mm x 6mm Timing Belt
 *    . ArxPax Circular Halbach Array (Alternate Solution) **Neodymium Permanent Magnet**
 *    . Small Neodymium Magnet attached to disc to pulse Hall-Effect Latch for RPM measurement
 *    . Plywood for display stand
 *    . Magnetic Viewing Film to view field lines
 *    . Breadboards
 */

#include <LiquidCrystal.h> 	//LCD library
#include <i2cmaster.h>		  //I2C interfacing library
//Consider using <Wire.h> for I2C instead

//Create object 'lcd1' with pin numbers as its parameters
LiquidCrystal lcd1(3, 5, 6, 7, 8, 9, 10, 11, 12, 13);

//Hall effect (US1881): measuring the time difference between two rising edges of the digital signal from sensor and then prints the rpm.
int val; 				        //value to compare
int prev_val = 1; 		  //previous value to compare
unsigned long t, cur_t; //time variables used for difference
unsigned int rpm = 0; 	//initialize rpm variable
 
void setup() {
  Serial.begin(9600); //UART serial communication
  lcd1.begin(16, 2); 	//Initialize 16 columns and 2 rows of LCD matrix
  i2c_init(); 			  //Initialize the I2C bus
  pinMode(2, INPUT); 	//Hall Effect Pin set to digital 2 as input
}

void loop() {
  //I2C bus init
  int dev = (0x5A << 1); 
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  //Read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); 	//Read 1 byte and then send ack
  data_high = i2c_readAck(); 	//Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  //Combines high and low bytes and processes temperature. MSB is a error bit and is ignored for temps.
  double tempFactor = 0.02; 	//0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; 	//Zero out the data
  int frac; 					        //Data after the decimal point

  //Mask off the error bit of the high byte, then shift left 8 bits and append the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = ((tempData * tempFactor) - 0.01);

  float celcius = (tempData - 273.15); 		    //Celcius
  float fahrenheit = ((celcius * 1.8) + 32); 	//Fahrenheit

  int val = digitalRead(2); 				    //Read value of hall sensor (digital 2)
  if ((prev_val == 1) && (val == 0)) { 	//Check for rising edge
    cur_t = micros(); 					        //Record time in microseconds since arduino program began running
    rpm = (1000000 * 60 / (cur_t - t)); //Calculate and print the rpm
    t = micros(); 						          //Record new time in us since program began for time difference
  }
  prev_val = val; 						          //Set new value for next iteration

  PORTC = ((1 << PORTC4) | (1 << PORTC5));//Enable 4.7k pullup resistors on SCL and SCA pins (I2C)
  lcd1.setCursor(0,0); 					          //Set cursor to starting position on 1st row of LCD
  lcd1.print("T: ");						          //Print T: header
  lcd1.print(fahrenheit); 		            //Print calculated value in Farenheit
  lcd1.print("F"); 						            //Print units
  lcd1.setCursor(0, 1); 			            //Set cursor to the starting position of the 2nd row of LCD
  lcd1.print("RPM: "); 				            //Print RPM: header
  lcd1.print(rpm); 						            //Print calculated value
  lcd1.clear(); 							            //Clear (refresh) LCD for the next iteration of data
}

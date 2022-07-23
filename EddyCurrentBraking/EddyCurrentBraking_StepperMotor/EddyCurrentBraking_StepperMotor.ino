/***********************************************
* @file  EddyCurrentBraking_StepperMotor.ino
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
*        braking. This application handles the stepper motor parameter
*        configuration and operation. Configured the stepper motor to act 
*        as a conventional DC motor by setting a constant RPM and high 
*        microstepping value in order to reduce oscillations in the system.
*        A greater microstepping value will yield higher RPM but reduce torque.
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

/* 
 *  SPI pin numbers (new terminology):
 *  SCK   13  // Serial Clock
 *  SDI   12  // Serial Data In
 *  SDO   11  // Serial Data Out
 *  CS    10  // Chip Select
 */
#include <SparkFunAutoDriver.h>     //Library that contains many functions used to set stepper motor parameters
#include <SPI.h>                    //SPI communication library
#include <SparkFundSPINConstants.h> //Library that contains many defined parameters for autodriver functions

//Initialize object 'boardA' of class AutoDriver
AutoDriver boardA(0, 10, 7, 8);     //Syntax: AutoDriver(int boardPos, int CSPin, int resetPin, int busyPin);

//GPIO macros
#define RST           7
#define BSY           8

//SPI redefines
#define CS            SS    //10
#define SDO           MOSI  //11
#define SDI           MISO  //12

void setup() {
  Serial.begin(9600);         //Set Baud Rate to 9600 (SPI is synchronous communication)
  
  //Setting up SPI and pin directions 
  //pinMode(BSY, INPUT); 	    //Busy pin signal can also be used to track RPM instead of Hall-Effect sensor
  pinMode(RST, OUTPUT);	      //Reset Pin
  pinMode(SDO, OUTPUT)
  pinMode(SDI, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);

  //Initialize pins
  digitalWrite(CS, HIGH);	    //Set CS to HIGH
  /* The following low/high transition resets the L6470 chip 
   * on the Autodriver board to be sure the Autodriver starts 
   * from a known state. 
   */
  digitalWrite(RST, LOW);     
  digitalWrite(RST, HIGH);   
							  
  SPI.begin();	              //Initialize SPI communication
  SPI.setDataMode(SPI_MODE3);  
  dSPINConfig();              //Configure motor behavior with chosen parameters
}

void loop() {
  boardA.run(REV, 2000);  //Run in reverse direction (arbitrary) at 2000 steps/second (indefinitely)
  //delay(5000);          //Wait 5 seconds
  //softStop();           //Braking by coasting (powering off motor)
  //hardStop();	          //Active braking by shorting coils together (good for emergency stop)
  //softHiZ();            //Execute 'soft stop' then put motor into high impedance (HiZ) state so motor may turn freely
  //hardHiZ();	          //Execute 'hard stop' then put motor into high impedance (HiZ) state so motor may turn freely
  //busyCheck();          //Check status of Busy pin 8 (could be used to measure RPM)
  //delay(1000);          //Wait 1 second
  /* Note: Additional library functions exist i.e. position tracking 
   * and speed control. Stepper motor is set to run at a constant RPM 
   * with high microstepping to minimize oscillations in our system 
   * that would further affect the epoxy and structure. 
   */
}

void dSPINConfig(void) {
  //First the object must know which SPI port to use -- some devices may have more than one
  boardA.SPIPortConnect(&SPI);      
  
  //Change these operation parameters as necessary -- SparkFundSPINConstants.h has supported macros listed
  boardA.configSyncPin(BUSY_PIN, 0);          //BUSY pin low during operations -- second parameter ignored in this mode
  boardA.configStepMode(STEP_FS);             //0 microsteps per step
  boardA.setMaxSpeed(20000);                  //20000 steps/s max
  boardA.setFullSpeed(20000);                 //Microstep below 20000 steps/s
  boardA.setAcc(20000);                       //Accelerate at 20000 steps/s/s (immediately reach max RPM)
  boardA.setDec(20000);                       //Decelerate at 20000 steps/s/s (immediately reach 0 RPM)
  boardA.setSlewRate(SR_530V_us);             //Upping the edge speed increases torque
  boardA.setOCThreshold(OC_1125mA);           //OC (Overcurrent) threshold 1125mA
  boardA.setPWMFreq(PWM_DIV_2, PWM_MUL_2);    //31.25kHz PWM freq
  boardA.setOCShutdown(OC_SD_DISABLE);        //Don't shutdown on overcurrent
  boardA.setVoltageComp(VS_COMP_DISABLE);     //Don't compensate for motor voltage
  boardA.setSwitchMode(SW_USER);   			      //Switch is not hard stop
  boardA.setOscMode(INT_16MHZ_OSCOUT_16MHZ);  //Internal crystal oscillator -> 16MHz clock.
 
  //Global scaling of current used during the following four conditions: (kvalInput range -> 0-255)
  //Good for reducing power consumption if full torque is not required
  boardA.setAccKVAL(128);           
  boardA.setDecKVAL(128);			
  boardA.setRunKVAL(128);			
  boardA.setHoldKVAL(32);                     //This controls the holding current; keep it low.
}

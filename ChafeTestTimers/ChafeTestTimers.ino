/***********************************************
* @file  ChafeTestTimers.ino
* @brief Testbench automation project for Arduino MEGA 2560
*        using timer interrupts to control speed & direction 
*        of stepper motor, as well as pump operation, with 
*        additional hardware interrupts configured to IR switches 
*        used for boundary detection.
*        
* @author Oliver Moore
* @version 1.0
***********************************************/

/*  __Testing of stepper motor and pump timer operation with IR limit switches__
 *  Hardware:
 *    . Arduino Mega2560
 *    . 2HSS60 Hybrid Stepper Servo Drive + Stepper motor
 *    . (x2) HiLetgo TCRT5000 IR photoelectric switch
 *    . Breakout board with L293D Motor Driver IC + pump
 *    
 *    Using Timer1 interrupt to pulse stepper motor, 
 *    Timer3&4 interrupts for pump OFF(30s)/ON(5s), and
 *    hardware interrupts on pins D2/D3 for IR switches 
 *    to toggle stepper direction at boundaries for cyclical
 *    lateral motion. Stepper motor configured to obtain 1Hz 
 *    shaft frequency (1s period), and 5000 cycles per test.
 */
// These macros must be placed before #include "TimerInterrupt.h"
#define TIMER_INTERRUPT_DEBUG     2   //Change to 0 to remove debugging print statements

#define USE_TIMER_1     true    //16-bit stepper pulse timer (0.2ms or 200us)
#define USE_TIMER_2     false
#define USE_TIMER_3     true    //16-bit pump off timer (30s)
#define USE_TIMER_4     true    //16-bit pump on timer (5s)
#define USE_TIMER_5     false

#include "TimerInterrupt.h"

/* GPIO Definitions */
//2HSS60 Hybrid Servo Driver
const int stepperStepPin = 5;
const int stepperEnPin = 6;
const int stepperDirPin = 7;

//TCRT5000 Infrared Photoelectric Switch
//Hardware Interrupt Pins (options: 2,3,18,19,20,21)
const int ir_sw0 = 2;
const int ir_sw1 = 3;

//L293D Motor Driver IC
const int pumpEnPin = 8;
const int pumpInPin1 = 9;
const int pumpInPin2 = 10;
const int LED = 12;

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Using 5000 microsteps/revolution setting: 
 *    SW3(OFF), SW4(OFF), SW5(ON), SW6(OFF) 
 * 5000Hz pulse frequency @ 5000 microsteps/rev -> 1Hz shaft rpm (1s period)
 * * * * * * * * * * * * * * * * * * * * * * * * * * */
float stepperPulse_freq = 5000.0;       //stepper pulse freq 5000hz = 200us (0.2ms)
int pumpOFFduration_millis = 30000;     //pump off for 30000ms       (30s) 
int pumpONduration_millis = 5000;       //pump on for 5000ms         (5s)

volatile int cycle_count = 0;
const int threshold = 5000;

//Stepper Pulse
#if USE_TIMER_1
    #define TIMER1_INTERVAL_MS    1
    #define TIMER1_FREQUENCY      stepperPulse_freq
    #define TIMER1_DURATION_MS    0   //duration = 0 or unspecified => run indefinitely
    void Timer1Handler() {
        #if (TIMER_INTERRUPT_DEBUG > 1)
            Serial.print("ITimer1 called, millis() = "); 
            Serial.println(millis());
        #endif
        run_stepper();
    }
#endif

//Unused
#if USE_TIMER_2
    #define TIMER2_INTERVAL_MS    10000
    #define TIMER2_FREQUENCY      (float) (1000.0f / TIMER2_INTERVAL_MS)
    #define TIMER2_DURATION_MS    0   //duration = 0 or unspecified => run indefinitely
    void Timer2Handler() {
        #if (TIMER_INTERRUPT_DEBUG > 1)
            Serial.print("ITimer2 called, millis() = ");
            Serial.println(millis());
        #endif  
    }
#endif

//Pump OFF
#if USE_TIMER_3 
    #define TIMER3_INTERVAL_MS    pumpOFFduration_millis
    #define TIMER3_FREQUENCY      (float) (1000.0f / TIMER3_INTERVAL_MS)
    #define TIMER3_DURATION_MS    0   //duration = 0 or unspecified => run indefinitely
    void Timer3Handler() {
        #if (TIMER_INTERRUPT_DEBUG > 1)
            Serial.print("ITimer3 called, millis() = ");
            Serial.println(millis());
        #endif
        activate_pump();
    }
#endif

//Pump ON
#if USE_TIMER_4
    #define TIMER4_INTERVAL_MS    pumpONduration_millis
    #define TIMER4_FREQUENCY      (float) (1000.0f / TIMER4_INTERVAL_MS)
    #define TIMER4_DURATION_MS    0   //duration = 0 or unspecified => run indefinitely
    void Timer4Handler() {
        #if (TIMER_INTERRUPT_DEBUG > 1)
            Serial.print("ITimer4 called, millis() = ");
            Serial.println(millis());
        #endif
        deactivate_pump();
    }
#endif

//Unused
#if USE_TIMER_5 
    #define TIMER5_INTERVAL_MS    10000
    #define TIMER5_FREQUENCY      (float) (1000.0f / TIMER5_INTERVAL_MS)
    #define TIMER5_DURATION_MS    0   //duration = 0 or unspecified => run indefinitely
    void Timer5Handler() {
        #if (TIMER_INTERRUPT_DEBUG > 1)
            Serial.print("ITimer5 called, millis() = ");
            Serial.println(millis());
        #endif
    }
#endif

void run_stepper() {
    //Pulse Stepper
    digitalWrite(stepperStepPin, !digitalRead(stepperStepPin)); //Toggle Stepper Motor Pin
}

void activate_pump() {
    //Activate Pump
    digitalWrite(pumpEnPin, HIGH); //Turn on Pump
    digitalWrite(LED, HIGH);       //Turn on LED
    ITimer3.disableTimer();        //Stop Pump OFF timer
    ITimer4.enableTimer();         //Start Pump ON timer
}

void deactivate_pump() {
    //Deactivate Pump
    digitalWrite(pumpEnPin, LOW);   //Turn off Pump
    digitalWrite(LED, LOW);         //Turn off LED
    ITimer4.disableTimer();         //Stop Pump ON timer
    ITimer3.enableTimer();          //Start Pump OFF timer
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    Serial.print(F("\nStarting TimerDuration on "));
    Serial.println(BOARD_TYPE);
    Serial.println(TIMER_INTERRUPT_VERSION);
    Serial.print(F("CPU Frequency = ")); 
    Serial.print(F_CPU / 1000000); 
    Serial.println(F(" MHz"));
    
    //Setup the Stepper Controller Pins as OUTPUTS
    pinMode(stepperStepPin, OUTPUT);
    pinMode(stepperDirPin, OUTPUT);
    pinMode(stepperEnPin, OUTPUT);
    
    //Setup the Pump Controller Pins as OUTPUTS
    pinMode(pumpInPin1, OUTPUT);
    pinMode(pumpInPin2, OUTPUT);
    pinMode(pumpEnPin, OUTPUT);
    pinMode(LED, OUTPUT);
    
    //Setup the IR Switch Pins as INPUT_PULLUP
    pinMode(ir_sw0, INPUT_PULLUP);
    pinMode(ir_sw1, INPUT_PULLUP);
    
    //Stepper Init
    delayMicroseconds(5); //ENA must be ahead of DIR by at least 5us
    digitalWrite(stepperDirPin, LOW); //Set the motor driection CCW
    digitalWrite(stepperEnPin, HIGH); //RUN the motor
    digitalWrite(LED, LOW); //Turn off LED
    
    //Pump Init
    digitalWrite(pumpInPin1, LOW);  // Set Pump Direction input
    digitalWrite(pumpInPin2, HIGH); // Set Pump Direction input

    //IR Switch Init
    digitalWrite(ir_sw0, HIGH); //Set the signal to HIGH
    digitalWrite(ir_sw1, HIGH); //Set the signal to HIGH
    
    //IR Switch Interrupt Config
    attachInterrupt(digitalPinToInterrupt(ir_sw0), IR0_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(ir_sw1), IR1_ISR, FALLING);

    /*  Timer0 is used for micros(), millis(), delay(), etc and can't be used.
        Select Timer 1-2 for UNO, 1-5 for MEGA, 1,3,4 for 16u4/32u4.
        Timer 1:   16-bit timer
        Timer 2:   8-bit timer
        Timer 3-5: 16-bit timers 
    */

    //Stepper Pulse
    #if USE_TIMER_1
        ITimer1.init();
        //Using atmega2560 used in MEGA 2560 => 16MHz CPU clock
        if(ITimer1.setFrequency(TIMER1_FREQUENCY, reinterpret_cast<timer_callback_p>(Timer1Handler), 0/*NULL*/, TIMER1_DURATION_MS)) {
            Serial.print(F("Starting  ITimer1 OK, micros() = ")); Serial.println(micros());
        }
        else
            Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
        ITimer1.enableTimer(); //Timer1 (Stepper Pulse) enabled at start
    #endif

    //Unused
    #if USE_TIMER_2
        ITimer2.init();
        if(ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS, Timer2Handler, TIMER2_DURATION_MS)) {
            Serial.print(F("Starting  ITimer2 OK, millis() = ")); Serial.println(millis());
        }
        else
            Serial.println(F("Can't set ITimer2. Select another freq. or timer"));
    #endif

    //Pump OFF
    #if USE_TIMER_3
        ITimer3.init();
        if(ITimer3.attachInterruptInterval(TIMER3_INTERVAL_MS, Timer3Handler, TIMER3_DURATION_MS)) {
            Serial.print(F("Starting  ITimer3 OK, millis() = ")); Serial.println(millis());
        }
        else
            Serial.println(F("Can't set ITimer3. Select another freq. or timer"));
        ITimer3.enableTimer(); //Timer3 (Pump OFF) enabled at start
    #endif

    //Pump ON
    #if USE_TIMER_4
        ITimer4.init();
        if(ITimer4.attachInterruptInterval(TIMER4_INTERVAL_MS, Timer4Handler, TIMER4_DURATION_MS)) {
            Serial.print(F("Starting  ITimer4 OK, millis() = ")); Serial.println(millis());
        }
        else
            Serial.println(F("Can't set ITimer4. Select another freq. or timer"));
        ITimer4.disableTimer(); //Timer4 (Pump ON) disabled at start
    #endif

    //Unused
    #if USE_TIMER_5
        ITimer5.init();
        if(ITimer5.attachInterruptInterval(TIMER5_INTERVAL_MS, Timer5Handler, TIMER5_DURATION_MS)) {
            Serial.print(F("Starting  ITimer5 OK, millis() = ")); Serial.println(millis());
        }
        else
            Serial.println(F("Can't set ITimer5. Select another freq. or timer"));
    #endif
}

void loop() {
    //Empty
}

void IR0_ISR() {
    //Turn off Interrupt0, Turn on Interrupt1
    detachInterrupt(digitalPinToInterrupt(ir_sw0));
    attachInterrupt(digitalPinToInterrupt(ir_sw1), IR1_ISR, FALLING);
    //Toggle direction
    digitalWrite(stepperDirPin, !digitalRead(stepperDirPin));
    
    //Avoid using blocking Serial.Print() functions within an ISR
    cycle_count++;
    if(cycle_count >= threshold) {
      digitalWrite(stepperEnPin, LOW); //stop the motor
    }
}

void IR1_ISR() {
    //Turn off Interrupt1, Turn on Interrupt0
    detachInterrupt(digitalPinToInterrupt(ir_sw1));
    attachInterrupt(digitalPinToInterrupt(ir_sw0), IR0_ISR, FALLING);
    //Toggle direction
    digitalWrite(stepperDirPin, !digitalRead(stepperDirPin));
}

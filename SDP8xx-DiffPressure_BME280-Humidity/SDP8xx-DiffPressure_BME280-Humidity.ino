/***********************************************
* @file SDP8xx-DiffPressure_BME280-Humidity.ino
* @brief LCD Display of SDP8xx Differential Pressure 
*        Sensor and BME280 Humdidity Sensor data.
* @author Oliver Moore
* @version 1.0
***********************************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <sdpsensor.h>

#define SEALEVELPRESSURE_HPA (1013.25)

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
Adafruit_BME280 bme;
SDP8XXSensor sdp;

unsigned long delayTimems;

void setup() {
    lcd.init();
    lcd.backlight();
    
    Serial.begin(115200);
    delay(1000); // let serial console settle

    BME280Init(); 
    SDP8Init();

    //roughly 1 second between sets of data points
    delayTimems = 1000;
}

void loop() { 

    //printBME280ValuesConsole();
    //printSDP8ValuesConsole();
    
    printBME280CSV();
    printSDP8CSV():
    
    printBME280ValuesLCD();
    printSDP8ValuesLCD();
    
    delay(delayTimems);
    //lcd.clear();
}

/* ******************* Functions ******************* */ 

/* ********* Initializations ********* */ 
void BME280Init(){
    //BME280 Sensor Init
    bool ret = bme.begin();  
    if (!ret) {
        Serial.println("BME280 Init Failed. Please check the address or the wiring!");
        while(1);
    }
    
    Serial.println("BME280 Init Success");
    Serial.println("Temperature,Pressure,Humidity");
}

void SDP8Init(){
    //SDP8 Differential Pressure Sensor Init
    do {
      int ret = sdp.init();
      if (ret == 0) {
        Serial.print("SDP8 init(): success\n");
        break;
      } else {
        Serial.print("SDP8 init(): failed, ret = ");
        Serial.println(ret);
        delay(1000);
      }
    } while(true);
}

/* ************ Print to Console ************ */ 
void printBME280ValuesConsole() {
    Serial.print("Temperature:");
    Serial.print(bme.readTemperature());
    Serial.print("C   ");
    
    Serial.print("Pressure:");
    Serial.print(bme.readPressure()/100.0F);
    Serial.print("hPa   ");
    
    Serial.print("Humidity:");
    Serial.print(bme.readHumidity());
    Serial.print("%   ");

    //extra
    //Serial.print("Altitude:");
    //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    //Serial.println("m  ");
}

void printSDP8ValuesConsole() {
int ret = sdp.readSample();
    if (ret == 0) {
      Serial.print(" Differential_Pressure:");
      Serial.print(sdp.getDifferentialPressure());
      Serial.print("Pa ");
      
      Serial.print(" Temperature:");
      Serial.print(sdp.getTemperature());
      Serial.print("C\n");
    } else {
      Serial.print("Error in readSample(), ret = ");
      Serial.println(ret);
    }
}

/* ************ Format for CSV File ************ */ 
/*   Generate data roughly once per second.
 *   T_BME,P_BME,H_BME,T_SDP,PDiff_SDP
 *   x0,y0,z0,a0,b0
 *   x1,y1,z1,a1,b1
 * 
 */
void printBME280CSV() {
    Serial.print(" ");
    Serial.print(",");
    Serial.print(bme.readTemperature());
    Serial.print(",");
    Serial.print(bme.readPressure()/100.0F);
    Serial.print(",");
    Serial.print(bme.readHumidity());
    Serial.print(",");
}

void printSDP8CSV() {
    int ret = sdp.readSample();
    if (ret == 0) {
      Serial.print(sdp.getTemperature());
      Serial.print(",");
      Serial.println(sdp.getDifferentialPressure());
    } else {
      Serial.print("Error in readSample(), ret = ");
      Serial.println(ret);
    }
}

/* ************ Print to LCD ************ */ 
/*  ---Refer to this for positioning---
 *  LCD Display Configuration
 *    01234567890123456789
 *  0 BME280  P:xxxx.yyhPa     20 chars 
 *  1    T:xx.yyC H:xx.yy%     20 chars
 *  2 SDP8  DiffP:-xx.yyPa     20 chars
 *  3       T:xx.yyC           14 chars
 */
 
void printBME280ValuesLCD() {
    lcd.setCursor(0,0);
    lcd.print("BME280  ");
    lcd.print("P:");
    lcd.print((bme.readPressure()/100.0F),2); //xxxx.yy 
    lcd.print("hPa");
    
    lcd.setCursor(0,1);
    lcd.print("T:");
    lcd.print(bme.readTemperature(),2); //xx.yy
    lcd.print("C ");
    lcd.print("H:");
    lcd.print(bme.readHumidity(),2); //xx.yy
    lcd.print("%");
}

void printSDP8ValuesLCD() {
 int ret = sdp.readSample();
    if (ret == 0) {
      lcd.setCursor(0,2);
      lcd.print("SDP8 ");
      lcd.print("DiffP:");
      lcd.print(sdp.getDifferentialPressure(),2); //-xx.yy (19-20 chars) or xx.yy (18-19 chars) depending on sign and number of x places. 
      lcd.print("Pa");
      
      lcd.setCursor(0,3);
      lcd.print("T:");
      lcd.print(sdp.getTemperature(),2); //xx.yy
      lcd.print("C");
    } else {
      lcd.setCursor(0,2);
      lcd.print("Error, ret=");
      lcd.print(ret);
    }
}

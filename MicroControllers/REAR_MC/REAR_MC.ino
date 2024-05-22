// Code for the Rear Micro Controller. Written by Tom Brouwers
// If you have any questions please email me at tommytom2006@icloud.com

#include <Wire.h>

//ADC Setup
#include<ADS1115_WE.h> 
ADS1115_WE adc_1 = ADS1115_WE(0x48);
ADS1115_WE adc_2 = ADS1115_WE(0x49);

// PINS
const int RPM_PIN = 4;
const int THROTTLE_PIN_IN = 32; // ADC where Potentiometer is connected to
const int MOTOR_CONTROLLER_PWM = 23; 

// Variables
volatile int RPM = 0;
volatile unsigned long lowTimeRPM = 0; //Volatile as it is used in interrupt
volatile unsigned long prevTimeRPM = 0;

float throttle_perc = 0;

//Main Data Variables

//ADC_1
float Bat1V  = 0.0;
float Bat2V = 0.0;
float motorTemp = 0.0;
float controllerTemp = 0.0;

//ADC_2
float current = 0.0;
float internalBatV = 0.0;
float placeholder_other = 0.0;


// Serial Read Vars
String data;
int MODE = 0;

// ---- INTERRUPT FUNCTIONS ----
 
void IRAM_ATTR RPM_Func(){ // Gets called when D4 is high (When the hall effect sensor senses a magnetic field)
  
  unsigned long curTimeRPM = millis();
  if (curTimeRPM - lowTimeRPM > 40){ // If time between previous 'HIGH' state of the sensor is greater than 100 ms (Debouncing)
    //Serial.println("RPM update");
    RPM = (60000/(curTimeRPM - prevTimeRPM)); // 1 Minute (in ms) over time of one rotation (ms) | if more magnets on driveshaft, divide by number of magnets
    lowTimeRPM = curTimeRPM;
    prevTimeRPM = curTimeRPM;
  }
  
} 

// ---- RUN ON INITIALIZATION ----

void setup() {
  Wire.begin();
  Serial.begin(115200); // Communication to front MC

  pinMode(RPM_PIN, INPUT);
  attachInterrupt(RPM_PIN, RPM_Func, RISING);
  
  //Setup the connection to the Motor Controller - We are using the ledc functionality of the ESP32 to generate a PWM signal
  ledcAttachPin(MOTOR_CONTROLLER_PWM, 0); // Pin, Channel 
  ledcSetup(0, 1000, 8); // Channel, Frequency (Hz), Resolution (Bits)

    // - ADC Setup 
  if(!adc_1.init()){
    Serial.println("ADS1115 not connected!");
  }
  adc_1.setVoltageRange_mV(ADS1115_RANGE_4096);
  adc_1.setCompareChannels(ADS1115_COMP_0_GND);
  adc_1.setMeasureMode(ADS1115_CONTINUOUS);

  if(!adc_2.init()){
    Serial.println("ADS1115 not connected!");
  }
  adc_2.setVoltageRange_mV(ADS1115_RANGE_4096);
  adc_2.setCompareChannels(ADS1115_COMP_0_GND);
  adc_2.setMeasureMode(ADS1115_CONTINUOUS);
}

void loop() {

  if (digitalRead(RPM_PIN) == HIGH){ // Stops 'bouncing' of the hall effect sensor. The reason why there is a difference between 'prevTime' and 'lowTime' is because the former is the time the moment a magnet was detected, while the latter changes to the latest time a magnet was detected. Ask Tom Brouwers for more information if necessary.
    lowTimeRPM = millis();
  } else if (millis() - lowTimeRPM > 3000){
    RPM = 0;
  }
  
  GET_THROT_PERC();
  READ_SERIAL();

  DRIVE_CONTROLLER();
  SEND_SERIAL();
  READ_ADC();
    
  //delay(0);
}

// ---- HELPER FUNCTIONS ----


// -- Reading sensors
void GET_THROT_PERC() {
  int throttle = analogRead(THROTTLE_PIN_IN);
  throttle_perc = (throttle - 900) / (2866.0 - 900);  // Find perc range = ((input - min) * 100) / (max - min) | For the min value, we would want it to be slightly higher than the lowest measurable value to avoid any jitter.
  if (throttle_perc < 0) {
    throttle_perc = 0;
  } else if (throttle_perc > 1) {
    throttle_perc = 1;
  }
}

void READ_ADC(){
  // ADC pulled to ground reads the temperature and battery V
  // 0 to bat 1v
  // 1 to bat 1+2v
  // 2 to mtemp
  // 3 to mc temp

  // ADC where address is pulled High reads the difference between ref and current and internal battery V and actual bat V

  // Use of voltage deviders, the formula is: Vout = (Vin * R2)/(R1+R2)
  // Thus Vin = (Vout*(R1+R2))/R2

  // -- ADC_1 --
  adc_1.setCompareChannels(ADS1115_COMP_0_GND);
  Bat1V = adc_1.getResult_V();  // V Devider of 33 <-> 100, so multiply by 4.030303
  adc_1.setCompareChannels(ADS1115_COMP_1_GND);
  Bat2V = adc_1.getResult_V() - Bat1V; // V Devider of 20 <-> 100 so multiply by
    //Find a conversion to convert the following voltage to temperature, and tune it
  adc_1.setCompareChannels(ADS1115_COMP_2_GND);
  motorTemp = adc_1.getResult_V();
  adc_1.setCompareChannels(ADS1115_COMP_3_GND);
  controllerTemp = adc_1.getResult_V();

  //-- ADC_2 --
  adc_2.setCompareChannels(ADS1115_COMP_0_1);
  current = (adc_2.getRawResult()/100.0)-0.01; // And add an offset if neccessary
  // 4096 mv mapped to -32768 to 32768 (Signed 16 bit value, signed means it has a bit signifying + -)
  // 0625 mv = 50 amps  
  // (625/4096)*32768 = 5000
  // (Vin/4096)*32768 = RawResult
  //  RawResult / 100 = Current in sensor
  delay(1);
  adc_2.setCompareChannels(ADS1115_COMP_2_GND);
  internalBatV = adc_2.getResult_V()*4.030303; //Voltage devider of 33 <-> 100, so multiply by 4.030303
  delay(1);
  //adc_2.setCompareChannels(ADS1115_COMP_3_GND);
  placeholder_other = 0; //adc_2.getResult_mV(); // Future Expansion port. REMEMBER TO REDUCE MAXIMUM POSSIBLE VIN TO BE LOWER THAN 4.2V USING VOLTAGE DEVIDERS - NOT INCLUDED YET
}


// -- Motor Controller

void DRIVE_CONTROLLER(){
  ledcWrite(0, map(throttle_perc*100,0,100,0,255)); // Creates the PWM signal - Channel, Value in assigned resolution
}

// -- SERIAL

void SEND_SERIAL(){ // Sends data to front microcontroller

  Serial.print("T");
  Serial.print(throttle_perc);
  Serial.print("R");
  Serial.print(RPM); 
  Serial.print("I");
  Serial.print(current);
  Serial.print("b");
  Serial.print(Bat1V);
  Serial.print("B");
  Serial.print(Bat2V);
  Serial.print("M");
  Serial.print(motorTemp);
  Serial.print("C");
  Serial.print(controllerTemp);
  Serial.print("i");
  Serial.print(internalBatV);
  Serial.print("P");
  Serial.println(placeholder_other);
  /*
  Serial.print("T");
  Serial.print(throttle_perc);
  Serial.print(",");
  Serial.print(RPM); 
  Serial.print(",");
  Serial.print(current);
  Serial.print(",");
  Serial.print(Bat1V);
  Serial.print(",");
  Serial.print(Bat2V);
  Serial.print(",");
  Serial.print(motorTemp);
  Serial.print(",");
  Serial.print(controllerTemp);
  Serial.print(",");
  Serial.print(internalBatV);
  Serial.print(",");
  Serial.println(placeholder_other);
  */
}

// Read data from other microcontroller
void READ_SERIAL() { // Reads the throttle Mode
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    if (incomingChar == '\n') {
      // Newline character detected, break the loop and print the received data
      break;
    }
  }

  // Check if there's serial data available
  if (Serial.available()) {
    data = Serial.readStringUntil('\n');  // Read data until a newline character
    int M_pos = data.indexOf("M");
    if (M_pos != -1) {
      MODE = data.substring(M_pos).toFloat();

    } else {
     // You can set default values or handle errors as needed
    }
  } else {
    
  }
  while (Serial.available() > 30) {  // Solves some buffer overflow issues
    char incomingChar = Serial.read();
  }
}

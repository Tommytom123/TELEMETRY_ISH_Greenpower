// Code for the rear Micro Controller. Written by Tom Brouwers
// Please ONLY edit inside the function for your task. If you want to edit other things, please ask Tom.

#include <Wire.h>

// PINS
const int RPM_PIN = 5;
const int THROTTLE_PIN_IN = 32; // ADC where Potentiometer is connected to
const int MOTOR_CONTROLLER_PWM = 27;

// Variables
volatile int RPM = 0;
volatile unsigned long lowTimeRPM = 0; //Volatile as it is used in interrupt
volatile unsigned long prevTimeRPM = 0;

float throttle_perc = 0;

/*
//Main Data Variables
motorTemp
controllerTemp
Bat1V
Bat2V 
CurrentDraw
*/

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
  Serial.begin(9600); // Communication to front MC

  pinMode(RPM_PIN, INPUT);
  attachInterrupt(RPM_PIN, RPM_Func, RISING);
  
  //Setup the connection to the Motor Controller - We are using the ledc functionality of the ESP32 to generate a PWM signal
  ledcAttachPin(MOTOR_CONTROLLER_PWM, 0); // Pin, Channel 
  ledcSetup(0, 1000, 8); // Channel, Frequency (Hz), Resolution (Bits)

  Serial.println(F("Gyro Setup"));
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

    
  delay(10);
}

// ---- HELPER FUNCTIONS ----


// -- Reading sensors
void GET_THROT_PERC() {
  int throttle = analogRead(THROTTLE_PIN_IN);
  throttle_perc = (throttle - 50) / (4095.0 - 50);  // Find perc range = ((input - min) * 100) / (max - min) | For the min value, we would want it to be slightly higher than the lowest measurable value to avoid any jitter.
  if (throttle_perc < 0) {
    throttle_perc = 0;
  }
}

void READ_ADC(){
  // Reads the ADC values 
  // Temperature 1
  // Temp 2
  // Bat V 1
  // Bat V 2
  // Current
  // Throttle?
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
  Serial.println(RPM);
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

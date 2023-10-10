#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GY521.h"
#include <SD.h>


//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//SD Setup
File myFile;
bool noSD = true;



//RPM Vars
const int rpmPin = 2;
const float wheelCirc = 0.002*60; //Wheel circumfrence in km * 60 (*60 to convert to km/h from km/m)
volatile unsigned long lowTimeRPM = 0; //Volatile as it is used in interrupt
volatile unsigned long prevTimeRPM = 0;
volatile int RPM = 0;


//Gyro Vars
GY521 sensor(0x69); //If AD0 Connected to GND, 0x68, if connected to VCC 0x69
//const int btnPin = 2;
volatile float angleGZ = 0;
volatile float angleGY = 0;
volatile float angleGX = 0;

//Loop Vars
int counter = 0;
int prevtime = 0;
int curtime = 0;

//Other Vars
int throttlePin = A0;
int throttle = 0;
float throttlePerc = 0;
const int potPin = 0;

void setup() {
  delay(100);
  Serial.begin(115200);
  // ---- Interupt and Pin Setup ----
  //pinMode(btnPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(btnPin), resetGyro, RISING);
  
  pinMode(potPin, OUTPUT);

  pinMode(rpmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rpmPin), RPM_Func, RISING);

  analogWriteRange(1024); // Default is 8 bit, 0-254
  analogWriteFreq(1000); //Hz

  // ---- Screen Setup ----
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println(F("Screen Setup"));
  
  delay(1000); // Pause for 1 second

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.println("SETUP");
  display.display();
  delay(1000);
  // ---- Screen Setup ----
  while (sensor.wakeup() == false)
  {
    Serial.print(millis());
    Serial.println(F("\tCould not connect to GY521"));
    delay(1000);
  }
  sensor.setAccelSensitivity(0);  //  2g
  sensor.setGyroSensitivity(0);   //  250 degrees/s
  sensor.setThrottle();

  // Set calibration
  sensor.gxe = 1;
  sensor.gye = 1;
  sensor.gze = 0;
  Serial.println(F("Gyro Setup"));

  // ---- SD Setup ----
  if (!SD.begin(15)) {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("SD Setup");

  
}


void loop() { //One loop takes about 30ms
  display.clearDisplay();
  curtime = millis();
  sensor.read();
  int gx = sensor.getGyroX();
  int gy = sensor.getGyroY();
  int gz = sensor.getGyroZ();

  int deltaTime = curtime - prevtime;
  float deltaGZ = deltaTime * gz;
  angleGZ += deltaGZ / 1000;
  float deltaGY = deltaTime * gy;
  angleGY += deltaGY / 1000;
  float deltaGX = deltaTime * gx;
  angleGX += deltaGX / 1000;

  display.setCursor(0,0);
  display.println(millis());
  display.print(F("X Rot:"));
  display.println(angleGX);
  display.print(F("Y Rot:"));
  display.println(angleGY);
  display.print(F("Z Rot:"));
  display.println(angleGZ); 
  display.print(F("RPM:"));
  display.println(RPM); 
  display.print(F("Speed:"));
  display.println((RPM*wheelCirc)); 
  throttle = analogRead(throttlePin);
  throttlePerc = (throttle - 40) / (1024.0 - 40); // Find perc range = ((input - min) * 100) / (max - min) | For the min value, we would want it to be slightly higher than the lowest measurable value to avoid any jitter.
  display.print(F("Throttle:"));
  display.println(throttlePerc); 

  analogWrite(potPin, throttle);
  
  if(counter % 3 == 0){ // Runs this every 3 loops, resulting in about 

    myFile = SD.open("TestVals.csv", FILE_WRITE);
    if (myFile) {
        myFile.print(millis());
        myFile.print(",");
        myFile.print(angleGX);
        myFile.print(",");
        myFile.print(angleGY);
        myFile.print(",");
        myFile.print(angleGZ);
        myFile.print(",");
        myFile.println(RPM); 
        //Serial.println("Update SD");
        noSD = false;
      } else {
        //Serial.println("SD Failed");
        noSD = true;
      }
    myFile.close();
    
  }
  if(noSD == true){
    display.println("NO SD CARD");
  }
  
  if (digitalRead(rpmPin) == HIGH){ // Stops 'bouncing' of the hall effect sensor. The reason why there is a difference between 'prevTime' and 'lowTime' is because the former is the time the moment a magnet was detected, while the latter changes to the latest time a magnet was detected. Ask Tom Brouwers for more information if necessary.
    lowTimeRPM = millis();
  } else if (millis() - lowTimeRPM > 3000){
    RPM = 0;
  }

  counter++;
  prevtime = curtime;
  display.display();
  //delay(30); 
} //Total delays: 30 ms

//HELPER FUNCTIONS

/*

void recSD(DateTime now){
  

}
*/

//INTERUPT FUNCTIONS

ICACHE_RAM_ATTR void resetGyro(){
  angleGZ = 0;
  angleGY = 0;
  angleGX = 0;
  Serial.println("Reset");
} 

ICACHE_RAM_ATTR void RPM_Func(){ // Gets called when D4 is high (When the hall effect sensor senses a magnetic field)
  
  unsigned long curTimeRPM = millis();
  if (curTimeRPM - lowTimeRPM > 40){ // If time between previous 'HIGH' state of the sensor is greater than 100 ms (Debouncing)
    //Serial.println("RPM update");
    RPM = (60000/(curTimeRPM - prevTimeRPM)); // 1 Minute (in ms) over time of one rotation (ms) | if more magnets on driveshaft, divide by number of magnets
    lowTimeRPM = curTimeRPM;
    prevTimeRPM = curTimeRPM;
  }
  
} 


#include <GY521.h>
#include <Wire.h>


const int rpmPin = 2;
volatile unsigned long lowTimeRPM = 0; //Volatile as it is used in interrupt
volatile unsigned long prevTimeRPM = 0;
volatile int RPM = 0;

int prevtime = 0;
int curtime = 0;

//Gyro Vars
GY521 sensor(0x69); //If AD0 Connected to GND, 0x68, if connected to VCC 0x69
//const int btnPin = 2;
volatile float angleGZ = 0;
volatile float angleGY = 0;
volatile float angleGX = 0;

  


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();

  pinMode(rpmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rpmPin), RPM_Func, RISING);
  

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
  sensor.axe = -0.0131885;
  sensor.aye = 0.0028174;
  sensor.aze = -1.0326611;
  sensor.gxe = 1.8501527;
  sensor.gye = 1.1008397;
  sensor.gze = 0.2435878;
  Serial.println(F("Gyro Setup"));
}


void loop() {
  // put your main code here, to run repeatedly:
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

    if (digitalRead(rpmPin) == HIGH){ // Stops 'bouncing' of the hall effect sensor. The reason why there is a difference between 'prevTime' and 'lowTime' is because the former is the time the moment a magnet was detected, while the latter changes to the latest time a magnet was detected. Ask Tom Brouwers for more information if necessary.
    lowTimeRPM = millis();
  } else if (millis() - lowTimeRPM > 3000){
    RPM = 0;
  }
  //Serial.println("Start");
  Serial.print("Z");
  Serial.print(angleGZ);
  Serial.print("Y");
  Serial.print(angleGY);
  Serial.print("X");
  Serial.print(angleGX);
  Serial.print("R");
  Serial.println(RPM);

  prevtime = curtime;
  delay(10);
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
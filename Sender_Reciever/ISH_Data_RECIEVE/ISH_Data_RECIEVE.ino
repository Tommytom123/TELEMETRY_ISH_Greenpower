#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#define SERIAL_BUFFER_SIZE 64


//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//PINS
const int THROTTLE_PIN_IN = A0;
const int BTN_PIN_LEFT = 0;
const int BTN_PIN_RIGHT = 2;
//const int MODE_SWITCH = ; // Commented out as I need more pins :\ . Likely a 3 way switch where we can change the car driving mode - Max Efficiency, Slow, Max Speed

//SD Setup
File myFile;
bool noSD = true;

//RPM Vars
float throttlePerc = 0;

//Loop Vars
int counter = 0;
volatile int curTime = 0;
volatile int debounceTime = 0;
int page = 0;
float X, Y, Z;
int R;
String data;


void setup() {
  delay(100);
  Serial.begin(9600);
  Wire.begin();
  // ---- Interupt and Pin Setup ----

  pinMode(BTN_PIN_LEFT, INPUT_PULLUP);
  pinMode(BTN_PIN_RIGHT, INPUT_PULLUP);
  //pinMode(MODE_SWITCH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN_LEFT), BTN_PRESSED_LEFT, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN_RIGHT), BTN_PRESSED_RIGHT, RISING);

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
  
  // ---- SD Setup ----
  if (!SD.begin(15)) {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("SD Setup");

  
}

void loop() { //One loop takes about 30ms
  display.clearDisplay();
  
  SERIAL_READ();  
  if(counter % 500 == 0){ // Only runs every 500 cycles
    INIT_SD();
  }

  if(counter % 3 == 0){  
    WRITE_SD();
  }

  DISPLAY_FUNC();
  counter++;
  display.display();
}


//HELPER FUNCTIONS
void DISPLAY_FUNC(){
  display.setCursor(0,0);
  
  if (page == 0){ //Page 1 --
    
    display.println(millis());
    display.println(X);
    display.println(Y);
    display.println(Z);
    display.println(R);
    GET_THROT_PERC();
    display.print(F("Throttle:"));
    display.println(throttlePerc); 

    if(digitalRead(BTN_PIN_LEFT) == LOW){
      display.println("Left BTN HIGH");
    }
    if(digitalRead(BTN_PIN_RIGHT) == LOW){
      display.println("Right BTN HIGH");
    }
  
  } else if(page == 1){ //Page 2
    
    display.println("Page 2");
    if(noSD) {
      display.println("NO SD CARD");
    }
  } else{ //Page 2
    display.println("ERROR with the page");
  }

  
}

void GET_THROT_PERC(){
  int throttle = analogRead(THROTTLE_PIN_IN);
  throttlePerc = (throttle - 50) / (1024.0 - 50); // Find perc range = ((input - min) * 100) / (max - min) | For the min value, we would want it to be slightly higher than the lowest measurable value to avoid any jitter.
  if (throttlePerc < 0) {
    throttlePerc = 0;
  }
}
// Sd card stuff
void INIT_SD(){
  if (!SD.begin(15)) {
    Serial.println("SD initialization failed!");
    noSD = true;
  } else {
    noSD = false; // SD card is detected
  }
}

void WRITE_SD(){
  // Check SD card status
  if (noSD == false) {
    myFile = SD.open("TestVals.csv", FILE_WRITE);
    if (myFile) {
      myFile.print(millis());
      myFile.print(",");
      myFile.print(X);
      myFile.print(",");
      myFile.print(Y);
      myFile.print(",");
      myFile.print(Z);
      myFile.print(",");
      myFile.println(R); 
    } else {
      noSD = true;
    }
    myFile.close();
  }
}
// Read data from other microcontroller
void SERIAL_READ() {
  while (Serial.available()>0) {
    char incomingChar = Serial.read();
    if (incomingChar == '\n') {
      // Newline character detected, break the loop and print the received data
      break;
    }
  } 
 

  // Check if there's serial data available
  if (Serial.available()) {
    data = Serial.readStringUntil('\n'); // Read data until a newline character
    int z_pos = data.indexOf("Z");
    int y_pos = data.indexOf("Y");
    int x_pos = data.indexOf("X");
    int r_pos = data.indexOf("R");

    if (z_pos != -1 && y_pos != -1 && x_pos != -1 && r_pos != -1) {
        Z = data.substring(z_pos + 1, y_pos).toFloat();
        Y = data.substring(y_pos + 1, x_pos).toFloat();
        X = data.substring(x_pos + 1, r_pos).toFloat();
        R = data.substring(r_pos + 1).toFloat();
    } else {
        // Handle the case where the string format is not as expected
        Z = Y = X = R = 0.0; // You can set default values or handle errors as needed
    }
  } else {
    Z = Y = X = R = 0.0;
  }
  while (Serial.available()>30) { // Solves some buffer overflow issues
    char incomingChar = Serial.read();
  }
}


// INTERUPTS FOR BUTTONS AND SUCH

ICACHE_RAM_ATTR void BTN_PRESSED_LEFT(){
  curTime = millis();
  if (curTime - debounceTime > 100){
    page += 1;
    debounceTime = curTime;
  }
    
}

ICACHE_RAM_ATTR void BTN_PRESSED_RIGHT(){
  int curTime = millis();
  if (curTime - debounceTime > 100){
    page += -1;
    debounceTime = curTime;
  }
}
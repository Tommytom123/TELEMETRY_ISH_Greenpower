// Code for the front Micro Controller. Written by Tom Brouwers
// Please ONLY edit inside the function for your task. If you want to edit other things, please ask Tom.
//test


#include <SD.h>
#include <FS.h>
#include <SPI.h>


#define SERIAL_BUFFER_SIZE 64

// Screen Setup

#include <Wire.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Chnage this to the correct display

#define ARRAYSIZE 120 // How wide the graphs are in pixels
#define PIXELRANGE 50 // How high the graphs are in pixels (Might want to change the naming convention here to make it consistent with above)
#define VOLTRANGE 12

// PINS
const int BTN_PIN_LEFT = 33;
const int BTN_PIN_RIGHT = 25;
const int MODE_PIN = 32; // Uses the adc and V deviders to allow for everything to work off of one singular pin :) Explaination should be on document later...

// const int MODE_SWITCH = ; // Commented out as I need more pins :\ .
// Likely a 3 way switch where we can change the car driving mode - Max Efficiency, Slow, Max Speed

// SD Setup
File myFile;
bool SD_in = false;
String FILE_NAME = "/telem_";
String F_EXTENSION = ".csv";

// Main Vars
int MODE = 0;
float throttle_perc = 0;
int RPM;

// Vars from the screen - Might want to improve naming for beter readibility later
int n=0;
int n1=0;
int j=0;
byte BatteryV[ARRAYSIZE];
float VoltMeasure=0;
int ScreenIndex = 0;

// Loop Vars
int counter = 0;
volatile int curTime = 0;
volatile int debounceTime = 0;
int page = 0;

String data;

//Second Serial Pins
#define RXD2 16
#define TXD2 17
int packet = 0;

// ---- INTERRUPT FUNCTIONS ----

void IRAM_ATTR BTN_PRESSED_LEFT() {
  curTime = millis();
  if (curTime - debounceTime > 100) {
    page += 1;
    debounceTime = curTime;
  }
}

void IRAM_ATTR BTN_PRESSED_RIGHT() {
  int curTime = millis();
  if (curTime - debounceTime > 100) {
    page += -1;
    debounceTime = curTime;
  }
}

// ---- RUN ON INITIALIZATION ----

void setup() {
  delay(100);
  Serial.begin(9600); // Communication to other board
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Communication to HC12. Lower this to the correct serial baud rate

  // - Interupt and Pin Setup -

  pinMode(BTN_PIN_LEFT, INPUT_PULLUP);
  pinMode(BTN_PIN_RIGHT, INPUT_PULLUP);
  //pinMode(MODE_SWITCH, INPUT_PULLUP);
  attachInterrupt(BTN_PIN_LEFT, BTN_PRESSED_LEFT, RISING);
  attachInterrupt(BTN_PIN_RIGHT, BTN_PRESSED_RIGHT, RISING);

  // - SD Setup -

  INIT_SD();

  // - Screen Setup -
  // New
  if (!u8g2.begin()){
    for(;;)
    ;
  }
  u8g2.clearBuffer();

  delay(1000);
}

// ---- MAIN LOOP ----

void loop() {
  //display.clearDisplay();
  u8g2.clearBuffer();

  SERIAL_READ();
  READ_MODE();

  PROCESS_DATA();

  if (counter % 3 == 0) {
    WRITE_SD();
  }

  DISPLAY_FUNC();

  counter++;
  u8g2.sendBuffer();
}

// ---- HELPER FUNCTIONS ----

void DISPLAY_FUNC() {
  float t = millis();
  float s = throttle_perc;//20;
  int b1 = 70;
  int b2 = 60;
  int ti = 60;
  int th = 1;
  switch (page){
    case 0: // Home page
      u8g2.setFont(u8g2_font_ncenB08_tr);

      // Display B1
      u8g2.setCursor(0, 45);
      u8g2.print("B1: ");
      u8g2.setCursor(20, 45);
      u8g2.print(b1);
      u8g2.print("%");

      // Display B2
      u8g2.setCursor(0, 60);
      u8g2.print("B2: ");
      u8g2.setCursor(20, 60);
      u8g2.print(b2);
      u8g2.print("%");

      // Display Power
      u8g2.setCursor(0, 25);
      u8g2.print("M: Power");

      // Display Ti
      u8g2.setCursor(60, 25);
      u8g2.print("Ti: ");
      u8g2.setCursor(80, 25);
      u8g2.print(ti);
      u8g2.print("min");

      /*// Display Temperature
      u8g2.setCursor(60, 20);
      u8g2.print("Temp: ");
      u8g2.setCursor(90, 20);
      u8g2.print(t);
      u8g2.cp437(true);
      u8g2.write(167);
      u8g2.print("C"); */

      // Display Speed
      u8g2.setCursor(60, 45);
      u8g2.print("S: ");
      u8g2.setCursor(72, 45);
      u8g2.print(s);
      u8g2.print(" k/h");

      // Display Th
      u8g2.setCursor(60, 60);
      u8g2.print("Th: ");
      u8g2.setCursor(78, 60);
      u8g2.print(th);
      u8g2.print("min");

      // Draw lines
      u8g2.drawLine(55, 0, 55, 64);
      u8g2.drawLine(0, 30, 128, 30);

      break;

    //case 1: // Settings?
      
      //break;
        
    case 2: // Graphs / data
      
        u8g2.drawFrame(2, 0, 126, 63);  // draw frame
        for(j=3;j<=n+3;j++) // From starting pixel to the ending pixel
        {
          u8g2.drawPixel(j,BatteryV[j]);
        }
      
      break;
    /*
    case 3: // Graphs / data
      break;
    */
    default:
      break;
      
  }
}

//-- Proccess Data

void PROCESS_DATA(){
  VoltMeasure=throttle_perc; //5+5*sin(n1*0.1);
  BatteryV[n]= map((throttle_perc*100),0,100,PIXELRANGE,0);//PIXELRANGE+3-VoltMeasure*PIXELRANGE/VOLTRANGE;    // convert voltage value into pixel height
  if(n==ARRAYSIZE-1){ 
    for(j=0;j<ARRAYSIZE;j++) BatteryV[j]=BatteryV[j+1]; // Shifts everything in the array over
  } 
    else {
      n++;
  }
  n1++; 
}

// -- Mode

void READ_MODE() {
  int m_analog = analogRead(MODE_PIN);
  // NOT FINAL VERSION AT ALL. Calculate threshold values based on the resistor values and what V they should result in in the 12? bit adc
  //display.println(m_analog);
  if (m_analog < 2000){
    MODE = 0;
  } else if (m_analog < 4095){
    MODE = 1;
  }
}

// -- Sd card stuff
void INIT_SD() {
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) { // SD CARD NOT FOUND
    Serial.println("No SD card attached");
    SD_in = false;
  } else { // SD CARD IS FOUND
    SD_in = true;
    // Count how many files are currently on the SD card
    File dir = SD.open("/");
    int num_files = 0;
    while (true) {
      File entry =  dir.openNextFile();
      if (!entry) { // No more files
        break;
      }
      Serial.print(entry.name());
      if (!entry.isDirectory()) {
        num_files++;
      }
      entry.close();
    }
    Serial.println(num_files);
    FILE_NAME = FILE_NAME + num_files + F_EXTENSION;
    Serial.println(FILE_NAME);
    // Create and initialize the telemetry file
    myFile = SD.open(FILE_NAME, FILE_WRITE);
    if (!myFile) {
      myFile.println("Time,Bat1V,Bat2V,Current");  // Initializes the data CSV with header
    }
    myFile.close();
    Serial.println("SD Setup");
  }
  // Currently no check to see how much space left, and to stop writing when a threshold is reached. Currently just ensure that the card is empty before running and hope for the best

  //SD_space = SD.cardSize() / (1024 * 1024);
  //Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void WRITE_SD() {

  if (SD_in == true) {
    myFile = SD.open(FILE_NAME, FILE_APPEND);
    if (myFile) {
      //display.println("WRITING");

      myFile.print(millis());
      myFile.print(",");
      myFile.print("Data,");
      myFile.println("WORKING");

      myFile.close();
    } else {
      SD_in = false;
      //display.println("SD CARD error");
    }
  }
}

// -- HC12

void TRANSMIT_DATA() {
  // Add the code to send data via the HC12 to the pit
  
  //int time_sec = millis() / 1000;
  Serial2.print("P"); // 
  Serial2.print(packet);
  Serial2.print("Data header");
  Serial2.println("The data itself");

  packet++; // Packet counter to detect if data transmission has been missed
  if (packet >= 100){
    packet = 0;
  }
}

// -- Serial

// Read data from other microcontroller
void SERIAL_READ() {
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
    int T_pos = data.indexOf("T");
    int R_pos = data.indexOf("R");

    if (T_pos != -1 && R_pos != -1) {
      throttle_perc = data.substring(T_pos + 1, R_pos).toFloat();
      RPM = data.substring(R_pos + 1).toFloat();
    } else {
      // Handle the case where the string format is not as expected
    }
  } else {
  }
  while (Serial.available() > 30) {  // Solves some buffer overflow issues
    char incomingChar = Serial.read();
  }
}

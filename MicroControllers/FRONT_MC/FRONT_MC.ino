// Code for the front Micro Controller. Written and compiled by Tom Brouwers. Elements by Elena and Alessandro.
// For use on the 2023/24 PCB boards.
// If you have any questions please email me at tommytom2006@icloud.com - or if ended up deprecating that one, please use tommaxbrouwers@gmail.com

// The code is split up into multiple files.

/*
- Current tasks in this code
    Find a consistent way to process the data into averages for the graphing, since we want to plot the past 30 minutes, not the past 62 incomming (Serial) data points
    
    Find a way to calculate the time remaining in the system according to the current draw, battery voltages, etc.
    Look at the graphs and find data that the driver - and pit - should know is abnormal and how to combat this during the race. This will likely require real world testing of the car.
    
    Variale naming conventions
    I have attempted to keep the code as strucutred and organized as possible, but there are certainly improvements to made.
    
    SD card reliability?
    Expand settings?
    Find more things to add? Such as more stuff for in car debugging?


- Variable and function naming schemes:
  This needs to seriously be cleaned up - not in a very consistent state currently and changes will need to be made.
    Variables referencing pins, all caps with _
    Variables present in the data collection (Over serial), fully UPPER
    Standard variables, camelcase

*/
#include <SD.h>
#include <FS.h>
#include <SPI.h>

#define SERIAL_BUFFER_SIZE 64

// Screen Setup

#include <Wire.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // Chnage this to the correct display
//U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

#define ARRAYSIZE 120  // How wide the graphs are in pixels
#define PIXELRANGE 62  // How high the graphs are in pixels

//Logging the performance
int prevTimeLoop = 0;
int FPS;

// PINS
const int BTN_PIN_NEXT = 33;
const int BTN_PIN_SELECT = 25;
const int BTN_PIN_UP = 26;
const int BTN_PIN_DOWN = 27;

const int MODE_PIN = 32;  // Uses the adc and V deviders to allow for everything to work off of one singular pin :) Explaination should be on document later...

const int CARD_DETECT_PIN = 13;

// const int MODE_SWITCH = ; // Commented out as I need more pins :\ .
// Likely a 3 way switch where we can change the car driving mode - Max Efficiency, Slow, Max Speed

// SD Setup
File myFile;
//bool SD_in = false;
String FILE_NAME = "/telem_";
String F_EXTENSION = ".csv";

// Main Vars
int MODE = 0; // Gets sent to rear microcontroller
int speed = 0; // KMPH

// - from serial
float throttle_perc = 0;
int RPM;
float current = 0.0;
float internalBatV = 0.0;
float Bat1V = 0.0;
float Bat2V = 0.0;
float MotorTemp = 0.0;
float controllerTemp = 0.0;
float placeholder_other = 0.0;

// Vars from the screen graphs
byte BatteryV_ARR[ARRAYSIZE]; // Plots the past 30 min of main battery V
//byte InternalBV[ARRAYSIZE]; // Plots the past 30 min of internal microcontroller battery Voltage
byte Current_ARR[ARRAYSIZE]; // Plots the current draw over the past 30 minutes
byte Speed_ARR[ARRAYSIZE];  // Plots the average speed over the past 30 minutes

int ScreenIndex = 0;

// Settings Variables
volatile int settingsLoc = 0;
volatile bool setting_up = false;
volatile bool setting_down = false;
volatile bool SELECTED = false;
const int LENGTH = 7;  // So its hard coded and we dont need to use some weird thing with sizeOf()

// Chnaging this bit requires changing some hard coded parameters further (and above) in the code
String settingPage[7][1] = {
  { "Set1" },
  { "HC12 Enabled" },
  { "HC12 Interval" },
  { "HC12 Channel" },
  { "SD Attached" },
  { "SD Enabled" }, 
  { "INFO" }
};
volatile int settingVals[7][1] = {
  { 0 },
  { 1 },     // 0 or 1
  { 1000 },  // Increment by 50 ms. Min 100, max 10000.
  { 100 },   // Increment by 1, min 0, max 124?
  { 0 },      // 0 or 1, indicating if SD card is registered and connected
  { 1 },     // 0 or 1
  { -1 }     //Placeholder
};

// Loop Vars
int counter = 0;
volatile int curTime = 0;
volatile int debounceTime = 0;
volatile int page = 0;

volatile int attempts = 2;

String data;

//Second Serial Pins
#define RXD2 16
#define TXD2 17
int packet = 0;
int prevTimeHC12 = 0;









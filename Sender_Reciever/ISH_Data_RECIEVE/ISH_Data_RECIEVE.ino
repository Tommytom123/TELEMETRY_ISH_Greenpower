// Code for the front Micro Controller. Written by Tom Brouwers
// Please ONLY edit inside the function for your task. If you want to edit other things, please ask Tom.

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>

#define SERIAL_BUFFER_SIZE 64

// Screen Setup
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// PINS
const int BTN_PIN_LEFT = 33;
const int BTN_PIN_RIGHT = 25;
const int MODE_PIN = 32; // Uses the adc and V deviders to allow for everything to work off of one singular pin :) Explaination should be on document

// const int MODE_SWITCH = ; // Commented out as I need more pins :\ .
// Likely a 3 way switch where we can change the car driving mode - Max Efficiency, Slow, Max Speed

// SD Setup
File myFile;
bool SD_in = false;

// Main Vars
int MODE = 0;
float throttle_perc = 0;
int RPM;

// Loop Vars
int counter = 0;
volatile int curTime = 0;
volatile int debounceTime = 0;
int page = 0;

String data;

//Second Serial Pins
#define RXD2 16
#define TXD2 17


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
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Communication to HC12

  // - Interupt and Pin Setup -

  pinMode(BTN_PIN_LEFT, INPUT_PULLUP);
  pinMode(BTN_PIN_RIGHT, INPUT_PULLUP);
  //pinMode(MODE_SWITCH, INPUT_PULLUP);
  attachInterrupt(BTN_PIN_LEFT, BTN_PRESSED_LEFT, RISING);
  attachInterrupt(BTN_PIN_RIGHT, BTN_PRESSED_RIGHT, RISING);

  // - SD Setup -

  INIT_SD();

  // - Screen Setup -
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  Serial.println(F("Screen Setup"));

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.println("SETUP");
  display.display();

  delay(1000);
}

// ---- MAIN LOOP ----

void loop() {
  display.clearDisplay();

  SERIAL_READ();
  READ_MODE();
  if (counter % 3 == 0) {
    WRITE_SD();
  }

  DISPLAY_FUNC();

  counter++;
  display.display();
}


// ---- HELPER FUNCTIONS ----

void DISPLAY_FUNC() {
  display.setCursor(0, 0);

  switch (page){
    case 0: // Home page
      display.println(millis());
      display.println(RPM);
      display.println(MODE);
      display.print(F("Throttle:"));
      display.println(throttle_perc);

      if (digitalRead(BTN_PIN_LEFT) == LOW) {
        display.println("Left BTN HIGH");
      }
      if (digitalRead(BTN_PIN_RIGHT) == LOW) {
        display.println("Right BTN HIGH");
      }
      break;

    case 1: // Settings?
      display.println("Page 2");
      break;

    case 2: // Graphs / data
      display.println("Page 3");
      break;

    case 3: // Graphs / data
      display.println("Page 4");
      break;

    default:
      display.println("ERROR with the page");
      break;
  }
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

  // filename = "/Telem.csv" // We need a function here that automatically gets a new filename every time the ESP restarts, so it doesn't overwrite anything

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    SD_in = false;
  } else {
    SD_in = true;
    myFile = SD.open("/Telem.csv", FILE_WRITE);
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
    myFile = SD.open("/Telem.csv", FILE_APPEND);
    if (myFile) {
      display.println("WRITING");

      myFile.print(millis());
      myFile.print(",");
      myFile.print("Data,");
      myFile.println("WORKING");

      myFile.close();
    } else {
      SD_in = false;
      display.println("SD CARD error");
    }
  }
}

// -- HC12

void TRANSMIT_DATA() {
  // Add the code to send data via the HC12 to the pits
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

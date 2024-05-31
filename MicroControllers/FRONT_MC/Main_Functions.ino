
// Code for the front Micro Controller. Written and compiled by Tom Brouwers. Elements by Elena Bilz Fernandes and Alessandro Fini.
// For use on the 2023/24 PCB boards.
// If you have any questions please email me at tommytom2006@icloud.com - or if ended up deprecating that one, please use tommaxbrouwers@gmail.com

/*
Arduino compiling works by first combining all the programs in the folder,
starting with the program with the same name then continues with  the rest in alphabetical order
It then compiles everything like one big file

Remember that all variable delarations need to occur before this file (Thus in alphabeticaly lower variables)
*/

// ---- INTERRUPT FUNCTIONS ----

void IRAM_ATTR BTN_PRESSED_NEXT() {
  curTime = millis();
  if (curTime - debounceTime > 300) {
    page += 1;
    if (page > 5) {
      page = 0;
    }
    debounceTime = curTime;
  }
}

void IRAM_ATTR BTN_PRESSED_SELECT() {
  curTime = millis();
  if (curTime - debounceTime > 300) {
    if (page == 1) {
      if (settingsLoc == LENGTH - 1) {
        page = 10;  // This PAGE is out of range of the normaly accessible pages. Any button press would then result in it reverting back to the start menu
      } else {
        SELECTED = !SELECTED;
      }
    }
    debounceTime = curTime;
  }
}

void IRAM_ATTR BTN_PRESSED_UP() {
  curTime = millis();
  if (curTime - debounceTime > 300) {
    if (page == 1) {  // If on settings page
      if (SELECTED == true) {
        setting_up = true;
      } else {
        if (settingsLoc - 1 < 0) {
          settingsLoc = LENGTH - 1;
        } else {
          settingsLoc--;
        }
      }
    }
    debounceTime = curTime;
  }
}

void IRAM_ATTR BTN_PRESSED_DOWN() {
  curTime = millis();
  if (curTime - debounceTime > 300) {
    if (page == 1) {
      if (SELECTED == true) {
        setting_down = true;
      } else {
        if (settingsLoc + 1 >= LENGTH) {
          settingsLoc = 0;
        } else {
          settingsLoc++;
        }
      }
    }
    debounceTime = curTime;
  }
}

void setting_up_func() {
  // A switch case for each setting option - not perfect due to the repetitive nature, but it allows the best customizability
  switch (settingsLoc) {
    case 0:  // Curently Empty
      settingVals[settingsLoc][0]++;
      break;

    case 1:  // HC12 Connection
      settingVals[settingsLoc][0] = toggle(settingVals[settingsLoc][0]);
      break;

    case 2:  // HC12 Time Interval
      if (settingVals[settingsLoc][0] < 10000) {
        settingVals[settingsLoc][0] += 50;
      }
      break;

    case 3:  // HC12 Channel
      if (settingVals[settingsLoc][0] < 127) {
        settingVals[settingsLoc][0] += 1;
      }
      break;

    case 4:                          // SD card
      if (settingVals[4][0] == 0) {  // If there currently is no SD card detected
        INIT_SD();
      }
      break;

    case 5:  // SD card enable Logging
      settingVals[settingsLoc][0] = toggle(settingVals[settingsLoc][0]);
      break;
  }
  setting_up = false;
}

void setting_down_func() {

  // A switch case for each setting option - not perfect due to the repetitive nature, but it allows the best customizability
  switch (settingsLoc) {
    case 0:  // Curently Empty
      settingVals[settingsLoc][0]--;
      break;

    case 1:  // HC12 Connection
      settingVals[settingsLoc][0] = toggle(settingVals[settingsLoc][0]);
      break;

    case 2:  // HC12 Time Interval
      if (settingVals[settingsLoc][0] > 1000) {
        settingVals[settingsLoc][0] -= 50;
      }
      break;

    case 3:  // HC12 Channel
      if (settingVals[settingsLoc][0] > 0) {
        settingVals[settingsLoc][0] -= 1;
      }
      break;

    case 4:                          // SD card
      if (settingVals[4][0] == 0) {  // If there currently is no SD card detected
        INIT_SD();
      }
      break;

    case 5:  // SD card enable Logging
      settingVals[settingsLoc][0] = toggle(settingVals[settingsLoc][0]);
      break;
  }
  setting_down = false;
}


void IRAM_ATTR SD_Attempts() {
  attempts = 2;
}

// ---- RUN ON INITIALIZATION ----

void setup() {
  delay(100);
  Wire.begin();
  Serial.begin(115200);  // Communication to other board
  Serial.setRxBufferSize(1024);
  Serial.setTimeout(10);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // Communication to HC12. Lower this to the correct serial baud rate

  // - Interupt and Pin Setup -
  pinMode(BTN_PIN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PIN_SELECT, INPUT_PULLUP);
  pinMode(BTN_PIN_UP, INPUT_PULLUP);
  pinMode(BTN_PIN_DOWN, INPUT_PULLUP);
  pinMode(CARD_DETECT_PIN, INPUT_PULLUP);
  attachInterrupt(BTN_PIN_NEXT, BTN_PRESSED_NEXT, RISING);
  attachInterrupt(BTN_PIN_SELECT, BTN_PRESSED_SELECT, RISING);
  attachInterrupt(BTN_PIN_UP, BTN_PRESSED_UP, RISING);
  attachInterrupt(BTN_PIN_DOWN, BTN_PRESSED_DOWN, RISING);
  attachInterrupt(CARD_DETECT_PIN, SD_Attempts, RISING);

  // - SD Setup -
  delay(100);
  INIT_SD();
  delay(100);

  // - Screen Setup -
  // New
  if (!u8g2.begin()) {
    for (;;)
      ;
  }
  //u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.clearBuffer();

  // Have a splash screen of the SBX logo or smthng here
 // u8g2.setCursor(30, 30);
  //u8g2.print("SBX ENGINEERS!!");
  //u8g2.setCursor(30, 41);
  // u8g2.print("CAR INFOTAINMENT WOO!!");
  // u8g2.setBitmapMode(false /* solid */);
  // u8g2.drawBitmap(0, 0, bitMapW, bitMapH, bitMapStartup);

  //u8g2.sendBuffer();

  //Initialize arrays to all be filled with zero's
  for (int i = 0; i < ARRAYSIZE; i++) {
    BatteryV_ARR[i] = 5;
    //byte InternalBV[ARRAYSIZE];
    Current_ARR[i] = 5;
    Speed_ARR[i] = 5;
  }

  delay(100);  // Time to show the startup screen
  //u8g2.clearBuffer();
}

// ---- MAIN LOOP ----

void loop() {
  //display.clearDisplay();
  u8g2.clearBuffer();

  curTime = millis();
  FPS = (1000 / (curTime - prevTimeLoop));
  prevTimeLoop = curTime;
  Serial.print("Start");
  Serial.println(FPS);

  Serial.println(millis());
  SERIAL_READ();
  Serial.println(millis());
  READ_MODE();
  // Send data to rear microcontroller

  Serial.println(millis());
  PROCESS_DATA();
  Serial.println(millis());
  // More complex button press actions

  if (setting_up == true) {
    setting_up_func();
  }
  if (setting_down == true) {
    setting_down_func();
  }

  if (digitalRead(CARD_DETECT_PIN) == HIGH && settingVals[4][0] == 0 && counter % 1000 == 0 && attempts > 0) {
    INIT_SD();
    attempts--;
  }
  
  if (settingVals[5][0] == 1) {  // If logging enabled
    WRITE_SD();
  }
/*
  if (settingVals[1][0] == 1 && curTime - prevTimeHC12 >= settingVals[2][0]) {
    TRANSMIT_DATA();
  }
*/
  Serial.println(millis());

  DISPLAY_FUNC();
  Serial.println(millis());
  counter++;

  u8g2.sendBuffer();
  //Serial.print(counter);
}

// ---- HELPER FUNCTIONS ----

void DISPLAY_FUNC() {
  switch (page) {
    case 0:  // Home page
      u8g2.setFont(u8g2_font_5x8_tr);

      // Display B1
      u8g2.setCursor(0, 40);
      u8g2.print("B1: ");
      u8g2.setCursor(18, 40);
      u8g2.print(Bat1V);
      u8g2.print("%");

      // Display B2
      u8g2.setCursor(0, 49);
      u8g2.print("B2: ");
      u8g2.setCursor(18, 49);
      u8g2.print(Bat2V);
      u8g2.print("%");

      // Display internal Bat
      u8g2.setCursor(0, 58);
      u8g2.print("iB: ");
      u8g2.setCursor(20, 58);
      u8g2.print(internalBatV);
      u8g2.print("V");

      // Display MODE
      u8g2.setCursor(0, 9);
      u8g2.print("M:");
      u8g2.setCursor(18, 9);
      if (MODE == 0) {
        u8g2.print("POWER");
      } else if (MODE == 1) {
        u8g2.print("ECO");
      } else {
        u8g2.print("SLOW");
      }


      /*// Display Temperature
      u8g2.setCursor(60, 20);
      u8g2.print("Temp: ");
      u8g2.setCursor(90, 20);
      u8g2.print(t);
      u8g2.cp437(true);
      u8g2.write(167);
      u8g2.print("C"); */

      // Display Speed
      u8g2.setCursor(60, 40);
      u8g2.print("S: ");
      u8g2.setCursor(72, 40);
      u8g2.print(speed);
      u8g2.print(" k/h");

      // Display Time elapsed
      u8g2.setCursor(60, 10);
      u8g2.print("Thr: ");
      u8g2.setCursor(78, 10);
      u8g2.print(int(trunc(throttle_perc *100)));
      u8g2.print("%");

      // Display Time remaining
      u8g2.setCursor(60, 20);
      u8g2.print("Ti: ");
      u8g2.setCursor(78, 20);
      u8g2.print(int(millis() / 1000));
      u8g2.print(" S");

      // Display current
      u8g2.setCursor(60, 30);
      u8g2.print("I: ");
      u8g2.setCursor(78, 30);
      u8g2.print(current);

      // Display FPS
      u8g2.setCursor(60, 50);
      u8g2.print("FPS: ");
      u8g2.setCursor(83, 50);
      u8g2.print(FPS);

      u8g2.setCursor(60, 58);
      u8g2.print("SD: ");
      u8g2.setCursor(83, 58);
      u8g2.print(SD.cardType());


      // Draw lines
      u8g2.drawLine(50, 0, 50, 64);  // x1,y1,x2,y2

      u8g2.drawLine(0, 30, 50, 30);
      u8g2.drawLine(0, 10, 50, 10);

      break;

    // -- SETTINGS PAGE --
    case 1:
      for (int i = 0; i < 4; i++) {
        if (settingsLoc + i < LENGTH && settingsLoc + i >= 0) {
          u8g2.setCursor(10, ((i)*10) + 15);
          u8g2.print(settingPage[settingsLoc + i][0]);
          if (settingVals[settingsLoc + i][0] != -1) {
            u8g2.setCursor(105, ((i)*10) + 15);
            u8g2.print(settingVals[settingsLoc + i][0]);
          }
        }
      }
      u8g2.setCursor(2, 15);
      u8g2.print(">");
      if (SELECTED == true) {
        u8g2.drawLine(2, 7, 40, 7);
        u8g2.drawLine(2, 16, 40, 16);
      }
      u8g2.drawLine(100, 0, 100, 64);


      break;

    // -- GRAPHS --
    case 2:  // TOTAL BATTERY VOLTAGE
      u8g2.setFont(u8g2_font_5x7_tr);
      u8g2.setCursor(3, 10);
      u8g2.print("Total Bat V");
      u8g2.drawFrame(0, 0, 128, 64);
      for (int x = 0; x < ARRAYSIZE; x++) {  // From starting pixel to the ending pixel
        u8g2.drawPixel(x + 1, BatteryV_ARR[x]);
      }

      break;

    case 3:  // CURRENT
      u8g2.setFont(u8g2_font_5x7_tr);
      u8g2.setCursor(3, 10);
      u8g2.print("Current");
      u8g2.drawFrame(0, 0, 128, 64);
      for (int x = 0; x < ARRAYSIZE; x++) {  // From starting pixel to the ending pixel
        u8g2.drawPixel(x + 1, Current_ARR[x]);
      }

      break;

    case 4:  // Speed
      u8g2.setFont(u8g2_font_5x7_tr);
      u8g2.setCursor(3, 10);
      u8g2.print("Speed");
      u8g2.drawFrame(0, 0, 128, 64);
      for (int x = 0; x < ARRAYSIZE; x++) {  // From starting pixel to the ending pixel
        u8g2.drawPixel(x + 1, Speed_ARR[x]);
      }

      break;

    case 10:  // Page for the INFO panel - Includes debugging information, such as FPS
      u8g2.setFont(u8g2_font_5x7_tr);
      u8g2.setCursor(3, 10);
      u8g2.print("Info");
      u8g2.setCursor(3, 20);
      u8g2.print("Version:");
      u8g2.setCursor(40, 20);
      u8g2.print("Alpha release 0.0.1");

      // Display FPS
      u8g2.setCursor(60, 50);
      u8g2.print("FPS: ");
      u8g2.setCursor(83, 50);
      u8g2.print(FPS);

      // Thing about what other debugging info we might want to add here

    default:
      break;
  }
}

//-- Process Data
// Converts the raw data to the arrays for the graph
void PROCESS_DATA() {
  // THIS NEEDS MAJOR UPDATES TO ALLOW STUFF TO WORK OVER THE PAST 30 MINUTES INSTEAD OF THE PAST ARRAYSIZE datapoints

  speed = RPM * 60 * 0.001;                                                     // Gets the speed from the revolutions per minute, converting them into revolutions per hour, then multiplying that by circumference (KM) of wheel
  BatteryV_ARR[ARRAYSIZE] = map((throttle_perc * 100), 0, 100, PIXELRANGE, 2);  // convert voltage value into pixel height
  Speed_ARR[ARRAYSIZE] = map((speed * 100), 0, 100, PIXELRANGE, 2);
  for (int x = 0; x < ARRAYSIZE; x++) { BatteryV_ARR[x] = BatteryV_ARR[x + 1]; }  // Shifts everything in the array over
}

int toggle(int x) {
  if (x == 1) {
    return 0;
  }
  return 1;
}

// -- Mode

void READ_MODE() {
  int m_analog = analogRead(MODE_PIN);
  // NOT FINAL VERSION

  // This works as a voltage devider
  // Vin --> 10K -- Out1 -- 10K -- Out2 -- 10K -- Out3 -- GND
  // The switch switches between the 3 output posistions


  if (m_analog < 1500) {
    MODE = 2;
  } else if (m_analog < 3000) {
    MODE = 1;
  } else {
    MODE = 0;
  }
}

// -- Sd card

void INIT_SD() { // Very slow execution
  Serial.println("SD_connecting?");
  if (!SD.begin(5)) {
    //Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {  // SD CARD NOT FOUND
    //Serial.println("No SD card attached");
    settingVals[4][0] = 0;
  } else {  // SD CARD IS FOUND
    settingVals[4][0] = 1;
    // Count how many files are currently on the SD card
    File dir = SD.open("/");
    int num_files = 0;
    while (true) {
      File entry = dir.openNextFile();
      if (!entry) {  // No more files
        break;
      }
      //Serial.print(entry.name());
      if (!entry.isDirectory()) {
        num_files++;
      }
      entry.close();
    }
    //Serial.println(num_files);
    FILE_NAME = FILE_NAME + num_files + F_EXTENSION;
    //Serial.println(FILE_NAME);
    // Create and initialize the telemetry file
    myFile = SD.open(FILE_NAME, FILE_WRITE);
    if (!myFile) {
      myFile.println("Time,Bat1V,Bat2V,Current");  // Initializes the data CSV with header
    }
    myFile.close();
    //Serial.println("SD Setup");
  }
  // Currently no check to see how much space left, and to stop writing when a threshold is reached. Currently just ensure that the card is empty before running and hope for the best

  //SD_space = SD.cardSize() / (1024 * 1024);
  //Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void WRITE_SD() { // Makes the code run at ~ 18 FPS
  
  if (settingVals[4][0] == 1) {  // Sd card connected
    Serial.println("SD_Write");
    myFile = SD.open(FILE_NAME, FILE_APPEND);
    if (myFile) {
      //display.println("WRITING");

      myFile.print(millis());
      myFile.print(",");
      myFile.print(RPM);
      myFile.print(",");
      myFile.print(current);
      myFile.print(",");
      myFile.print(internalBatV);
      myFile.print(",");
      myFile.print(Bat1V);
      myFile.print(",");
      myFile.print(Bat2V);
      myFile.print(",");
      myFile.print(MotorTemp);
      myFile.print(",");
      myFile.print(controllerTemp);
      myFile.print(",");
      myFile.println(throttle_perc);
      //myFile.println("WORKING");

      myFile.close();
    } else {
      settingVals[4][0] = 0;  // If it can't write, sets the sd card to error
      //display.println("SD CARD error");
    }
  }
}

// -- HC12

void TRANSMIT_DATA() {
  //For any questions regarding data transmission, contact me at alessandro.l.fini@gmail.com
  // Add the code to send data via the HC12 to the pit lane
  if (settingVals[1][0] == 1) {
   //Serial2.print(millis());
    char hc12data[54];
    sprintf(hc12data, "%06i%06i%06i%06i%06i%06i%06i%06i%06i", packet, RPM, current, internalBatV, Bat1V, Bat2V, MotorTemp, controllerTemp, throttle_perc);
    //Serial2.print(millis());
    //Serial2.println(packet);
    // Serial2.println("The data itself");
    packet++;  // Packet counter to detect if data transmission has been missed
    if (packet >= 100) {
      packet = 0;
    }
  }
}

void FECencoder(char messagestring[msglen]) {
  //For any questions, contact me at alessandro.l.fini@gmail.com
  String data;
  //Setting array parameters for encoded output.
  char encoded[msglen + ECC_LENGTH];
  char message_frame[msglen];

  memset(message_frame, 0, sizeof(message_frame));                       // Clear the array
  for (uint i = 0; i <= msglen; i++) { message_frame[i] = message[i]; }  // Fill with the message

  //Encode input
  rs.Encode(message_frame, encoded);
  //Convert encoded output from char to string.
  for (uint i = 0; i < sizeof(encoded); i++) { data = data + encoded[i]; }

  //Send output string to escaping functions.
  byteEscaping(data);
}

void byteEscaping(String input) {
  String output;
  int len = input.length() + 1;
  int outlen = output.length() + 1;
  char inputarray[len];
  char outputarray[outlen];

  //Converting input string to char.
  input.toCharArray(inputarray, len);

  for (int i = 0; i < len - 1; i++) {

    //If the STARTBYTE is found mid message, it will add a second one next to it, and the receiving end will be programmed to ignore it.
    if (inputarray[i] == STARTBYTE) {
      output = output + inputarray[i];
      output = output + STARTBYTE;
    } else {
      output = output + inputarray[i];
    }
    if (i == len - 1) {
      break;
    }
  }

  output.toCharArray(outputarray, outlen);

  for (int i = 0; i < len - 1; i++) {

    //If the STARTBYTE is found mid message, it will add a second one next to it, and the receiving end will be programmed to ignore it.
    if (outputarray[i] == ENDBYTE) {
      output = output + outputarray[i];
      output = output + ENDBYTE;
    } else {
      output = output + outputarray[i];
    }
    if (i == len - 1) {
      break;
    }
  }
  packager(output);
}

void packager(String data) {
  Serial2.print(STARTBYTE);
  Serial2.print(data);
  Serial2.println(ENDBYTE);
}

// -- Serial

void SERIAL_SEND() {  // Send data to rear microcontroller
  Serial.print("M");
  Serial.println(MODE);  // I think thats all that needs sending?
}

// Read data from other microcontroller
// There are likely optimizations to be found here
void SERIAL_READ() {
  
  if (Serial.available()) {
    
    // Packet character order - T R I b B M C i P
    String incomingChar = Serial.readStringUntil('T');  // Clears buffer until the start of packet
    // Check if there's serial data available

    data = Serial.readStringUntil('\n');  // Read data until a newline character
    int T_pos = 0;                        //data.indexOf("T");
    int R_pos = data.indexOf("R");
    int I_pos = data.indexOf("I");
    int b_pos = data.indexOf("b");
    int B_pos = data.indexOf("B");
    int M_pos = data.indexOf("M");
    int C_pos = data.indexOf("C");
    int i_pos = data.indexOf("i");
    int P_pos = data.indexOf("P");

    if (T_pos != -1 && R_pos != -1 && I_pos != -1 && b_pos != -1 && B_pos != -1 && M_pos != -1 && C_pos != -1 && i_pos != -1 && P_pos != -1) {
      throttle_perc = data.substring(T_pos, R_pos).toFloat();
      RPM = data.substring(R_pos + 1, I_pos).toFloat();
      current = data.substring(I_pos + 1, b_pos).toFloat();
      Bat1V = data.substring(b_pos + 1, B_pos).toFloat();
      Bat2V = data.substring(B_pos + 1, M_pos).toFloat();
      MotorTemp = data.substring(M_pos + 1, C_pos).toFloat();
      controllerTemp = data.substring(C_pos + 1, i_pos).toFloat();
      internalBatV = data.substring(i_pos + 1, P_pos).toFloat();
      placeholder_other = data.substring(P_pos + 1).toFloat();


    } else {
      // Handle the case where the string format is not as expected
    }
  }
   else {
  }
  while (Serial.available() > 0) {  // Solves some buffer overflow issues
    Serial.println("Stuff");
    char incomingChar = Serial.read();
  }
}
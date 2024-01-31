

#include <Arduino.h>
#include <U8g2lib.h>


#define ARRAYSIZE 100
#define PIXELRANGE 50
#define VOLTRANGE 12

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


/////////////// Bitmap


const unsigned char upir_logo [] PROGMEM = {  
  0xEA, 0x3A, 0xAA, 0x28, 0x6A, 0x1A, 0x26, 0x2A, };


// 'icon_3dcube', 16x16px
const unsigned char bitmap_icon_3dcube [] PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0xE0, 0x06, 0x98, 0x18, 0x86, 0x60, 0x8A, 0x50, 
  0xA2, 0x45, 0x82, 0x40, 0xA2, 0x44, 0x82, 0x40, 0xA2, 0x45, 0x8A, 0x50, 
  0x86, 0x60, 0x98, 0x18, 0xE0, 0x06, 0x80, 0x01, };  
// 'icon_battery', 16x16px
const unsigned char bitmap_icon_battery [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x1F, 0x02, 0x20, 
  0xDA, 0x66, 0xDA, 0x66, 0xDA, 0x66, 0x02, 0x20, 0xFC, 0x1F, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
// 'icon_dashboard', 16x16px
const unsigned char bitmap_icon_dashboard [] PROGMEM = {
  0xE0, 0x07, 0x18, 0x18, 0x84, 0x24, 0x0A, 0x40, 0x12, 0x50, 0x21, 0x80, 
  0xC1, 0x81, 0x45, 0xA2, 0x41, 0x82, 0x81, 0x81, 0x05, 0xA0, 0x02, 0x40, 
  0xD2, 0x4B, 0xC4, 0x23, 0x18, 0x18, 0xE0, 0x07, };
// 'icon_fireworks', 16x16px


// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 384)
const unsigned char* bitmap_icons[3] = {
  bitmap_icon_3dcube,
  bitmap_icon_battery,
  bitmap_icon_dashboard
};


// 'scrollbar_background', 8x64px
const unsigned char bitmap_scrollbar_background [] PROGMEM = {
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
  0x00, 0x40, 0x00, 0x00, };


// 'item_sel_outline', 128x21px
const unsigned char bitmap_item_sel_outline [] PROGMEM = {
  0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0C, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0xF8, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 
  };



//////////////// End

#define BUTTON_SELECT_PIN 7 // pin for SELECT button
#define BUTTON_UP_PIN 4 // pin for UP button 
#define BUTTON_DOWN_PIN 5 // pin for DOWN button
#define BUTTON_CHANGE_PIN 8


 U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


const int NUM_ITEMS = 3; // number of items in the list and also the number of screenshots and screenshots with QR codes (other screens)
const int MAX_ITEM_LENGTH = 20; // maximum characters for the item name

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with item names
  { "Item1" }, 
  { "Mode" }, 
  { "Item3" }, 
 };

int button_up_clicked = 0; // only perform action when button is clicked, and wait until another press
int button_select_clicked = 0; // same as above
int button_down_clicked = 0; 

int item_selected = 0; // which item in the menu is selected
int item_sel_previous; // previous item - used in the menu screen to draw the item before the selected one
int item_sel_next; // next item - used in the menu screen to draw next item after the selected one
  




int n=0;
int n1=0;
int j=0;
byte BatteryV[ARRAYSIZE];
float VoltMeasure=0;
int ScreenIndex = 0;


int buttonState=0;            // the current reading from the input pin
int lastButtonState = HIGH;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;  





void setup(void) {

  u8g2.begin();
  pinMode(BUTTON_CHANGE_PIN, INPUT_PULLUP);

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP); // up button

  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button

  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP); // down button

  delay(1000);

}

int ti = 60;
int v1 = 100;
int v2 = 200;

void loop(void) {

    

  int reading = digitalRead(BUTTON_CHANGE_PIN);

   // If the switch changed, due to noise or pressing:
  if (reading ==LOW && lastButtonState==HIGH) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    buttonState=1;
  }

  if ((millis() - lastDebounceTime) > debounceDelay && buttonState==1) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading == LOW) {
      //buttonState = reading;
      ScreenIndex += 1; 
    if (ScreenIndex >= 3) {
        ScreenIndex = 0;
    }

    }
    buttonState=0;
    //lastDebounceTime = millis();
    //lastButtonState = reading;
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  //if (buttonState == LOW) {
    
  //  buttonState=HIGH;
  //}

  //u8g2.firstPage();
  //do {
    //u8g2.setFont(u8g2_font_ncenB14_tr);	// choose a suitable font
    //u8g2.drawStr(0,15,"Hello World!");	// write something to the internal memory
    //u8g2.sendBuffer();
  //  u8g2.drawFrame(2, 0, 128, 52);
  //  }while(u8g2.nextPage());
  //u8g2.clearDisplay();

  //Serial.println(random(0,10));
  //BatteryV[n]=random(0,11);

  //u8g2.firstPage();
  //do {
    if (ScreenIndex == 0){

    
  u8g2.clearBuffer();					// clear the internal memory
  
  //BatteryV[n]=5+5*sin(n1*0.1);
  VoltMeasure=5+5*sin(n1*0.1);
  //Serial.print("n = ");Serial.print(n);Serial.print(" ;   ->  Bat Volt = ");Serial.print(BatteryV[n]);
  BatteryV[n]=PIXELRANGE+3-VoltMeasure*PIXELRANGE/VOLTRANGE;    // convert voltage value into pixel height
  //Serial.print("  pixel height = ");Serial.println(BatteryV[n]);

  //testecheck=(5+5*sin(n1*0.1))*10;
  //teste=(5+5*sin(n1*0.1))*10;
  //Serial.print("  Byte value =");Serial.print(teste);Serial.print("  float value =");Serial.println(testecheck);
  //u8g2.clearBuffer();					// clear the internal memory
  u8g2.drawFrame(2, 0, 126, 63);  // draw frame
  for(j=0;j<=n;j++)
  {
    u8g2.drawPixel(j,BatteryV[j]);
  }
  
  /*for(j=n;j>=0;j--)
  {
    u8g2.drawPixel(ARRAYSIZE-1+j-n,BatteryV[j]);
  }
  for(j=n;j<ARRAYSIZE;j++)
  {
    u8g2.drawPixel(j-n,BatteryV[j]);
  }*/

  //u8g2.drawPixel(10,23);
  //u8g2.drawPixel(11,25);
  
  //u8g2.sendBuffer();
  //delay(100);
  
  //delay(1000);
  if(n==ARRAYSIZE-1){ for(j=0;j<ARRAYSIZE;j++) BatteryV[j]=BatteryV[j+1];} else n++;
  //if(n==ARRAYSIZE-1) n=0; else n++;
  n1++;  
  
  //} while(u8g2.nextPage());
  //u8g2.clearDisplay();

  u8g2.sendBuffer();					// transfer internal memory to the display
  //delay(500);

  }
  // From Adafruit Code



  
  if (ScreenIndex == 1) {
    changeDisplayContent();
  }

  if (ScreenIndex == 2) {
    menu(); // LOW = pressed

     if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
        item_selected = item_selected - 1; // select previous item
        button_up_clicked = 1; // set button to clicked to only perform the action once
        if (item_selected < 0) { // if first item was selected, jump to last item
          item_selected = NUM_ITEMS-1;
        }
      }

     else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to next menu item
        item_selected = item_selected + 1; // select next item
        button_down_clicked = 1; // set button to clicked to only perform the action once
        if (item_selected >= NUM_ITEMS) { // last item was selected, jump to first menu item
          item_selected = 0;
          }
      } 

      if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick 
        button_up_clicked = 0;
      }

      if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick
        button_down_clicked = 0;
      }

  }

if (item_selected == 0) {
  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // select button clicked, jump between screens
      button_select_clicked = 1; // set button to clicked to only perform the action once
     if (ScreenIndex == 2) {ScreenIndex = 3;} // menu items screen --> variable screen
     else if (ScreenIndex == 3) {ScreenIndex = 2;} // variable screen --> menu items screen
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // unclick 
    button_select_clicked = 0;
  }
}

if (item_selected == 1) {
  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // select button clicked, jump between screens
      button_select_clicked = 1; // set button to clicked to only perform the action once
     if (ScreenIndex == 2) {ScreenIndex = 4;} // menu items screen --> variable screen
     else if (ScreenIndex == 4) {ScreenIndex = 2;} // variable screen --> menu items screen
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // unclick 
    button_select_clicked = 0;
  }
}

if (item_selected == 2) {
  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // select button clicked, jump between screens
      button_select_clicked = 1; // set button to clicked to only perform the action once
     if (ScreenIndex == 2) {ScreenIndex = 5;} // menu items screen --> variable screen
     else if (ScreenIndex == 5) {ScreenIndex = 2;} // variable screen --> menu items screen
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // unclick 
    button_select_clicked = 0;
  }
}


  if (ScreenIndex == 3) 
  { 
    u8g2.clearBuffer();
    u8g2.setCursor(20, 45);
    u8g2.print("Variable: ");
    
    

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
        ti += 3;
        button_up_clicked = 1;
      }

      if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick 
        button_up_clicked = 0;
      } 

      //u8g2.print(ti);


    //u8g2.sendBuffer();

    if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to previous menu item
        ti -= 3;
        button_down_clicked = 1;
      }

      if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick 
        button_down_clicked = 0;
      } 

      u8g2.print(ti);


    u8g2.sendBuffer();

    
  }

    
  //}

  

  if (ScreenIndex == 4) 
  { 
    u8g2.clearBuffer();
    u8g2.setCursor(20, 45);
    u8g2.print("Variable1: ");

    if (v1 == 100) { 
        u8g2.print("low");
        u8g2.sendBuffer();
      }

    if (v1 == 103) { 
        u8g2.print("eco");
        u8g2.sendBuffer();
      }

    if (v1 == 106) { 
        u8g2.print("power");
        u8g2.sendBuffer();
      }

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
        if (v1 <= 103) {
          v1 += 3;
        }

        button_up_clicked = 1;
      }

      if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick 
        button_up_clicked = 0;
      } 

      //u8g2.print(ti);


    //u8g2.sendBuffer();

    if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to previous menu item
        if (v1 >= 103) {
          v1 -= 3;
        }
        //v1 -= 3;
        button_down_clicked = 1;
      }

      if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick 
        button_down_clicked = 0;
      } 

      //u8g2.print(v1);


    u8g2.sendBuffer();
  }



  if (ScreenIndex == 5) 
  { 
    u8g2.clearBuffer();
    u8g2.setCursor(20, 45);
    u8g2.print("Variable2: ");


     if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) { // up button clicked - jump to previous menu item
        v2 += 3;
        button_up_clicked = 1;
      }

      if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) { // unclick 
        button_up_clicked = 0;
      } 

      //u8g2.print(ti);


    //u8g2.sendBuffer();

    if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) { // down button clicked - jump to previous menu item
        v2 -= 3;
        button_down_clicked = 1;
      }

      if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) { // unclick 
        button_down_clicked = 0;
      } 

      u8g2.print(v2);


    u8g2.sendBuffer();
  }

  // set correct values for the previous and next items
  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {item_sel_previous = NUM_ITEMS - 1;} // previous item would be below first = make it the last
  item_sel_next = item_selected + 1;  
  if (item_sel_next >= NUM_ITEMS) {item_sel_next = 0;} // next item would be after last = make it the first

}


void changeDisplayContent() {
  // Clear the display
  u8g2.clearBuffer();

  int t = 25;
  int s = 20;
  int b1 = 70;
  int b2 = 60;
  int ti = 60;
  int th = 1;

  

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
  u8g2.setCursor(0, 20);
  u8g2.print("M: Power");

  // Display Ti
  u8g2.setCursor(60, 20);
  u8g2.print("Ti: ");
  u8g2.setCursor(80, 20);
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

  // Send the content to the display
  u8g2.sendBuffer();


  
}




void menu() {
  u8g2.setFont(u8g_font_7x14);
  u8g2.clearBuffer();

  //u8g2.setCursor(50, 12);
  //u8g2.print("Menu");


      // selected item background
      u8g2.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline);

      // draw previous item as icon + label
      u8g2.setFont(u8g_font_7x14);
      u8g2.drawStr(25, 15, menu_items[item_sel_previous]); 
      u8g2.drawXBMP( 4, 2, 16, 16, bitmap_icons[item_sel_previous]);          

      // draw selected item as icon + label in bold font
      u8g2.setFont(u8g_font_7x14B);    
      u8g2.drawStr(25, 15+20+2, menu_items[item_selected]);   
      u8g2.drawXBMP( 4, 24, 16, 16, bitmap_icons[item_selected]);     

      // draw next item as icon + label
      u8g2.setFont(u8g_font_7x14);     
      u8g2.drawStr(25, 15+20+20+2+2, menu_items[item_sel_next]);   
      u8g2.drawXBMP( 4, 46, 16, 16, bitmap_icons[item_sel_next]);  

      // draw scrollbar background
      u8g2.drawXBMP(128-8, 0, 8, 64, bitmap_scrollbar_background);

      // draw scrollbar handle
      u8g2.drawBox(125, 64/NUM_ITEMS * item_selected, 3, 64/NUM_ITEMS); 

   
        
       
  u8g2.sendBuffer(); // send buffer from RAM to display controller

/////////////
}

/*

  HelloWorld.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8g2lib.h>

/*#include <TimerInterrupt.hpp>         //https://github.com/khoih-prog/TimerInterrupt
#include <ISR_Timer.hpp>              //https://github.com/khoih-prog/TimerInterrupt

// Select the timers you're using, here ITimer1
#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false
#define USE_TIMER_4     false
#define USE_TIMER_5     false
*/
#define ARRAYSIZE 100
#define PIXELRANGE 50
#define VOLTRANGE 12

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//      U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// End of constructor list

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
  
  //Serial.begin(115200);
  u8g2.begin();
  pinMode(8, INPUT_PULLUP);

  // Init timer ITimer1
  //ITimer1.init();

  //u8g2.clearBuffer();					// clear the internal memory
  //u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  //u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  //u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1000);
  //randomSeed(analogRead(0));
}



void loop(void) {

  int reading = digitalRead(8);

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
    if (ScreenIndex >= 2) {
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

  // Send the content to the display
  u8g2.sendBuffer();



  
}


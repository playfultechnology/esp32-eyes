/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

// INCLUDES
#include "Face.h"
#include <Wire.h>
#include "Common.h"

// DEFINES
#define WIDTH  128 //180
#define HEIGHT 64 //240
#define EYE 40 //40

// GLOBALS
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 4, /* data= */ 5);
Face *face;
String emotionName[] = {
    "Normal",
    "Angry",
    "Glee",
    "Happy",
    "Sad",
    "Worried",
    "Focused",
    "Annoyed",
    "Surprised",
    "Skeptic",
    "Frustrated",
    "Unimpressed",
    "Sleepy",
    "Suspicious",
    "Squint",
    "Furious",
    "Scared",
    "Awe"
};

void setup(void) {
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  face = new Face(WIDTH, HEIGHT, EYE);

 // face->Expression.GoTo_Happy();
  face->Behavior.Clear();
	face->Behavior.SetEmotion(eEmotions::Glee, 1.0);

  // Unlike almost every other Arduino application, I2C address scanner etc., u8g2 library
  // requires 8-bit I2C address, so we shift the 7-bit address left by one.
  u8g2.setI2CAddress(0x3C<<1);
  u8g2.begin();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display
}

void loop(){
  static uint32_t counter = millis();
  static uint8_t emotionno = 0;
  static uint8_t emotionflag = 0;
  static uint8_t n = 0;

  if(millis() - counter > 6000) {
    counter = millis();
    emotionno = n++;
    if(n >= EMOTIONS_COUNT) n = 0;
      emotionflag = 1;
    }
  
  if(emotionflag) {
    emotionflag = 0;
    // Update the eyes' emotion
    face->Behavior.Clear();
    face->Behavior.SetEmotion((eEmotions)emotionno, 1.0);

    // Draw a text string 
    //u8g2.setCursor((WIDTH-(emotionName[emotionno].length() * 5)) / 2, HEIGHT);
    //u8g2.print(emotionName[emotionno]);
    //u8g2.sendBuffer();					// transfer internal memory to the display
    Serial.println(emotionName[emotionno]);
  }

  face->Update();
  delay(10);
}
/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

// INCLUDES
// Built-in Arduino I2C library
#include <Wire.h>
// Defines all face functionality
#include "Face.h"

// CONSTANTS
const byte blinkPin = 16;

// GLOBALS
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
  // Create a serial connection
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);

  pinMode(blinkPin, INPUT_PULLUP);

  // Create a new face
  face = new Face(/* screenWidth = */ 128, /* screenHeight = */ 64, /* eyeSize = */ 40);
  // Assign the current expression
  face->Expression.GoTo_Normal();

  // Assign a weight to each emotion that can be chosen
  face->Behavior.SetEmotion(eEmotions::Normal, 1.0);
  //face->Behavior.SetEmotion(eEmotions::Angry, 1.0);
  //face->Behavior.SetEmotion(eEmotions::Sad, 1.0);
  // Automatically select a random behaviour (based on the weight assigned to each emotion)
  face->RandomBehavior = true;

  // Automatically blink
  face->RandomBlink = true;
  // Set blink rate
  face->Blink.Timer.SetIntervalMillis(4000);
  //face->Blink.Timer.Stop();

  // Automatically choose a new random direction to look
  face->RandomLook = true;

}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  // Check if x is outside the input range
  if (x <= in_min) return out_min;
  if (x >= in_max) return out_max;

  // Calculate the proportion of x relative to the input range
  float proportion = (x - in_min) / (in_max - in_min);

  // Map the proportion to the output range and return the result
  return (proportion * (out_max - out_min)) + out_min;
}

void loop(){
  static int lastMoveTime;

  // To avoid making eyes too twitchy (and to allow time for previous animation to end), we only recalculate new position every 500ms
  if(millis() - lastMoveTime > 500) {
    int yRaw = analogRead(25);
    int xRaw = analogRead(26);
    float y = mapFloat(yRaw, 0, 4095, 1.0, -1.0);
    float x = mapFloat(xRaw, 0, 4095, 1.0, -1.0);
    face->Look.LookAt(x, y);
    lastMoveTime = millis();
  }
  if(!digitalRead(blinkPin)){
    face->DoBlink();
  }


  /*
  // Use this code to set a particular emotion from a button
  int32_t potValue = analogRead(15);
  float anger = map(potValue, 0, 4095, 0.0, 2.0);
  face->Behavior.SetEmotion(eEmotions::Angry, anger);
  */

  /*
  // Use this code to cycle automatically through all emotions
  static uint32_t counter = millis();
  static uint8_t emotionno = 0;
  static uint8_t changeEmotionFlag = 0;
  static uint8_t n = 0;

  if(millis() - counter > 6000) {
    counter = millis();
    emotionno = n++;
    if(n >= EMOTIONS_COUNT) n = 0;
      changeEmotionFlag = 1;
    }
  
  if(changeEmotionFlag) {
    // Clear any previously assigned weights
    face->Behavior.Clear();
    // Assign weight of 1.0 to the next emotion in the list
    face->Behavior.SetEmotion((eEmotions)emotionno, 1.0);
    // Print the new emotion to the serial monitor 
    Serial.println(emotionName[emotionno]);
    // Reset the flag
    changeEmotionFlag = 0;
  }
  */
  face->Update();
  //delay(10);
}
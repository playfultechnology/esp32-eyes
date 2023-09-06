/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses 
****************************************************/


#include "Face.h"
#include "Common.h"

Face::Face(uint16_t screenWidth, uint16_t screenHeight, uint16_t eyeSize) 
	: LeftEye(*this), RightEye(*this), Blink(*this), Look(*this), Behavior(*this), Expression(*this)
{
	Width = screenWidth;
	Height = screenHeight;
	EyeSize = eyeSize;

	CenterX = Width / 2;
	CenterY = Height / 2;

	LeftEye.IsMirrored = true;

	Behavior.Timer.Start();
}

void Face::LookFront() {
	Look.LookAt(0.0, 0.0);
}

void Face::LookRight() {
	Look.LookAt(-1.0, 0.0);
}

void Face::LookLeft() {
	Look.LookAt(1.0, 0.0);
}

void Face::LookTop() {
	Look.LookAt(0.0, 1.0);
}

void Face::LookBottom() {
	Look.LookAt(0.0, -1.0);
}

void Face::Wait(unsigned long milliseconds) {
	unsigned long start;
	start = millis();
	while (millis() - start < milliseconds) {
		Draw();
	}
}

void Face::DoBlink() {
	Blink.Blink();
}

void Face::Update() {
	if(RandomBehavior) Behavior.Update();
	if(RandomLook) Look.Update();
	if(RandomBlink)	Blink.Update();
	Draw();
}

void Face::Draw() {

  u8g2.clearBuffer();

  // Only clear the section of the screen with the eyes - not the text underneath!
  //u8g2.setDrawColor(0);
  //u8g2.drawBox(20, 0, 64, 128);
  //u8g2.setDrawColor(1);

	LeftEye.CenterX = CenterX - EyeSize / 2 - EyeInterDistance;
	LeftEye.CenterY = CenterY;
	LeftEye.Draw();

	RightEye.CenterX = CenterX + EyeSize / 2 + EyeInterDistance;
	RightEye.CenterY = CenterY;
	RightEye.Draw();

  //u8g2.setDisplayRotation(U8G2_R3);
  //_buffer.setPivot(_buffer.width(), 0);//_buffer.height()/2);
  //_buffer.pushRotated(270, -1);
	//_buffer.pushSprite(0, 0);

  u8g2.sendBuffer();					// transfer internal memory to the display
  //u8g2.setDisplayRotation(U8G2_R0);

}

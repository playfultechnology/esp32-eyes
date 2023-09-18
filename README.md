# esp32-eyes
Emotive animated eyes on an OLED display, as inspired by Anki Cozmo etc.

Inspired by the expressive eye animations of the Anki "Cozmo" robot, this Arduino/ESP8266/ESP32 library displays a set of animated eyes on a small 128x64 OLED screen.

<img src="https://github.com/playfultechnology/esp32-eyes/blob/main/doc/anki-cozmo-faces-3-1024x576.jpg" />


Unlike some libraries which display a set of pre-rendered bitmap images for each frame of animation, this library draws each frame dynamically from a programmatic set of parameters.

Heavily based on <a href="https://github.com/luisllamasbinaburo/ESP32_Faces/">this library</a>, although with significant adjustments:

 - OLED (using <a href="https://github.com/olikraus/u8g2">u8g2</a>) rather than TFT (based on <a href="https://github.com/Bodmer/TFT_eSPI">eSPI_TFT</a>)
 - Horizontal rather than vertical alignment
 - Mirrored left/right eye animations
 - Automatic or manual blink and look direction

## Component Structure

- *Face* is the core object, consisting of left and right eye components, behaviour, expression, a look assistant, and a blink assistant. The outputs of these components are chained together to create the display on each frame:

- The Face *Behaviour* contains an array of weighted _Emotions_. There are currently 18 emotions, as defined in the enum in FaceEmotions.hpp. 
Each emotion can have an individual weight assigned to it, as follows: 
```
face->Behavior.SetEmotion(eEmotions::Glee, 1.0);
face->Behavior.SetEmotion(eEmotions::Anger, 0.5);
```
If ```face->RandomBehavior = true;``` (which is the default), then Face will use a roulette-wheel selection to randomly assign a new emotion based on their relative weights.
The frequency with which the current emotion is re-evaluated is determined by the value passed to ```face->Behaviour.Timer.SetIntervalMillis();``` , although note that this does not always result in a change, since the same emotion may be selected as is currently active.
If no weights have been assigned to any emotion, the face will default to "normal" emotion.

Note that emotions are _not_ blended together - the face only ever expresses one emotion at a time. The "weight" assigned to each emotion does not specify how they are composited together, but rather it states the relative probability with which any given emotion will be selected.

- *Expression* handles transitions of the eyes to a selected emotion. Each emotion has a preset configuration that defines the eye height, slope, radius, and offset for each emotion (defined in ```EyePresets.h```), and these are assigned by the corresponding ```GoTo_Emotion()``` function.

- The Blink assistant handles blinking. If ```face->RandomBlink = true;``` (the default), the eyes will automatically blink, with a frequency determined by the value passed to ```face->Blink.Timer.SetIntervalMillis();```
The eyes may be blinked manually by calling ```face->DoBlink();```

- The Look assistant handles the global position of the eyes around the screen. If ```face->RandomLook = true;``` (the default), the eyes will randomly move around the screen, with a frequency determined by the value passed to ```face->Look.Timer.SetIntervalMillis();```
The eyes may be set to look at any position by calling ```face->Look.LookAt(x, y);``` (where x and y range from -1.0 - 1.0) 


- Each eye is responsible for managing their own drawing operations to the display (in EyeDrawer.cpp)



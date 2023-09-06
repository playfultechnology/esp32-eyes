# esp32-eyes
Emotive animated eyes on an OLED display, as inspired by Anki Cozmo etc.

Inspired by the expressive eye animations of the Anki "Cozmo" robot, this Arduino/ESP8266/ESP32 library displays a set of animated eyes on a small 128x64 OLED screen.
Unlike some libraries which require a set of bitmap images for each frame of animation, this library draws each frame dynamically from a programmatic set of parameters.

Heavily based on <a href="https://github.com/luisllamasbinaburo/ESP32_Faces/">this library</a>, although with significant adjustments:

 - OLED (using <a href="https://github.com/olikraus/u8g2">u8g2</a>) rather than TFT (based on <a href="https://github.com/Bodmer/TFT_eSPI">eSPI_TFT</a>)
 - Horizontal rather than vertical alignment
 - Mirrored left/right eye animations

#include "HID-Project.h"
#include <Adafruit_NeoPixel.h>
//I've modified the SimpleRotary library to reduce the debounce time to allow for a double click.  
#include "SimpleRotaryCRC.h"

#define buttonPlay 2
#define buttonForward 3
#define buttonBack 1
#define pixelPin 4 //built in 11
#define NUMPIXELS 2
#define rotaryCCW 9
#define rotaryCW 10
#define rotaryClick 8

int mutedBlinkDelay = 500;
long lastMutedBlink;
boolean mutedBlinkOnOff = false;
boolean muteBlinkMode = false;
boolean singleClick = false;
long lastSingleClick;
int singleClickTimeout = 650;
SimpleRotary rotary(rotaryCW, rotaryCCW, rotaryClick);

int StarleafColor[] = {0, 0, 255};//blue //zoom and starleaf have same mute control and the same colour coincidentally
int TeamsColor[] = {255, 0, 200};//purple
int WebexColor[] = {0, 255, 0};//green
int SkypeColor[] = {100, 100, 255};

int rainbowMillis_j;
int rainbowMillisDelay = 30;
long lastRainbowMillis;

Adafruit_NeoPixel pixels(NUMPIXELS, pixelPin, NEO_GRB + NEO_KHZ800);

int mode = 0;
int muteDelay = 500;
long lastMutePress;
boolean mutePressed = false;

int statusDelay = 1000;
long lastStatus;

void StartUpColorTest2(void);
void setPixels(int color0, int color1, int color2);

String inputString = "";
bool stringComplete = false;

bool connectedToComputer = false;

void serialEventRun(void) {
  if (Serial.available()) serialEvent();
}
void setup() {
  rotary.setErrorDelay(5);
 
  // rotary.setDebounceDelay(1);
  pinMode(buttonPlay, INPUT_PULLUP);
  pinMode(buttonForward, INPUT_PULLUP);
  pinMode(buttonBack, INPUT_PULLUP);
  Serial.begin(115200);
  inputString.reserve(200);

  // Sends a clean report to the host. This is important on any Arduino type.
  Consumer.begin();
  Keyboard.begin();
  pixels.begin();
  //run rainbow cycle quickly
  pixels.setBrightness(150);
  StartUpColorTest2(); // Startup Pixel debug Test
  setPixels(StarleafColor[0], StarleafColor[1], StarleafColor[2]);

}


void loop() {
  if (connectedToComputer) {
    SerialLoop();//advanced mode - uses Companion app
  }
  else {
    NoSerialLoop();//normal macro keyboard mode
  }
}
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      connectedToComputer = true;
      stringComplete = true;
    }
  }
}

void SerialLoop() {
  byte rotated = rotary.rotate();
  if (rotated == 1) {
    Consumer.write(MEDIA_VOL_UP);//volume up
    delay(25);
  }
  else if (rotated == 2) {
    Consumer.write(MEDIA_VOL_DOWN);//volume down
    delay(25);
  }
  if (!digitalRead(buttonPlay)) {

    // See HID Project documentation for more Consumer keys
    Consumer.write(MEDIA_PLAY_PAUSE);
    delay(300);
  }
  if (!digitalRead(buttonForward)) {
    Consumer.write(MEDIA_NEXT);
    delay(300);
  }
  if (!digitalRead(buttonBack)) {
    Consumer.write(MEDIA_PREVIOUS);
    delay(300);
  }
  byte i = rotary.pushType(1000);
  if (muteBlinkMode)
  {
    MutedLEDs();
  }
  else
  {
    if (mode == 0) {
      setPixels(StarleafColor[0], StarleafColor[1], StarleafColor[2]);
    }
    else if (mode == 1) {
      setPixels(TeamsColor[0], TeamsColor[1], TeamsColor[2]);
    }
    else if (mode == 2) {
      setPixels(WebexColor[0], WebexColor[1], WebexColor[2]);
    }
    else if (mode == 3) {
      setPixels(SkypeColor[0], SkypeColor[1], SkypeColor[2]);
    }
    else if (mode == -1) {
      rainbowMillis();
    }
  }
  if ( i == 1 ) {
    
    if (singleClick){
      Serial.println("DoubleClick");
      singleClick = false;
    }
    else {
      Serial.println("Marking SingleClick");
      singleClick = true;
      lastSingleClick = millis();
    }
  }
  else if ( i == 2 ) {
    Serial.println("MuteHold");
  }
  if ((singleClick) && ((millis() - lastSingleClick) > singleClickTimeout)){
    Serial.println("MutePress");
    muteBlinkMode = !muteBlinkMode;
    singleClick = false;
  }
  if ((millis() - lastStatus) > statusDelay) {
    Serial.println("Status");
    lastStatus = millis();
  }
  if (stringComplete) {
    //Serial.print("inputString:");
    //Serial.println(inputString);
    if (inputString.indexOf("MuteGo") > -1) {
      if (mode == 0) {
        Keyboard.press(KEY_LEFT_ALT);  //keyboard command for mute in Starleaf/Zoom Alt+A
        Keyboard.press('a');
      }
      else if (mode == 1) {
        Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for mute in Teams Ctrl+Shift+M
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('m');
      }
      else if (mode == 2) {
        Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for mute in Webex Ctrl + M
        Keyboard.press('m');
      }
      else if (mode == 3) {
        Keyboard.press(KEY_LEFT_GUI);  //keyboard command for mute in Skype, windows + F4
        Keyboard.press(KEY_F4);
      }
      delay(100);
      Keyboard.releaseAll();
    }
    if (inputString.indexOf("Mute:1") > -1) {
      muteBlinkMode = true;
    }
    if (inputString.indexOf("Mute:0") > -1) {
      muteBlinkMode = false;
    }
    if (inputString.indexOf("Mode:0") > -1) {
      mode = 0;
    }
    if (inputString.indexOf("Mode:1") > -1) {
      mode = 1;
    }
    if (inputString.indexOf("Mode:2") > -1) {
      mode = 2;
    }
    if (inputString.indexOf("Mode:3") > -1) {
      mode = 3;
    }
    if (inputString.indexOf("Mode:Off") > -1) {
      mode = -1;
    }
    String printString = "Mute:";
    printString += muteBlinkMode;
    printString += "|Mode:";
    printString += mode;
    Serial.println(printString);
    inputString = "";
    stringComplete = false;
  }
}
void NoSerialLoop() {
  byte rotated = rotary.rotate();
  if (rotated == 1) {

    Consumer.write(MEDIA_VOL_UP);//volume up
    delay(25);

  }
  else if (rotated == 2) {
    Consumer.write(MEDIA_VOL_DOWN);//volume down
    delay(25);
  }

  if (!digitalRead(buttonPlay)) {

    // See HID Project documentation for more Consumer keys
    Consumer.write(MEDIA_PLAY_PAUSE);
    delay(300);
  }
  if (!digitalRead(buttonForward)) {
    Consumer.write(MEDIA_NEXT);
    delay(300);
  }
  if (!digitalRead(buttonBack)) {
    Consumer.write(MEDIA_PREVIOUS);
    delay(300);
  }
  byte i = rotary.pushType(1000);
  if ( i == 1 ) {
    if (singleClick){

        if (mode == 0) {
      Keyboard.press(KEY_LEFT_ALT);  //keyboard command for video off in Starleaf Alt+V
      Keyboard.press('v');
    }
    else if (mode == 1) {
      Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for video off in Teams Ctrl+Shift+O
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('o');
    }
    else if (mode == 2) {
      Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for video off in Webex Ctrl+Shift+v
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('v');
    }
    else if (mode == 3) {
      Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for video off in Skype Ctrl+Shift+k
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('k');
    }
    delay(100);
    Keyboard.releaseAll();
    singleClick = false;
    }
    else{
      singleClick = true;
      lastSingleClick = millis();
    }
  }

  if ((singleClick) && ((millis() - lastSingleClick) > singleClickTimeout)){
    if (mode == 0) {
      Keyboard.press(KEY_LEFT_ALT);  //keyboard command for mute in Starleaf Alt+A
      Keyboard.press('a');
    }
    else if (mode == 1) {
      Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for mute in Teams Ctrl+Shift+M
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('m');
    }
    else if (mode == 2) {
      Keyboard.press(KEY_LEFT_CTRL);  //keyboard command for mute in Webex Ctrl + M
      Keyboard.press('m');
    }
    else if (mode == 3) {
      Keyboard.press(KEY_LEFT_GUI);  //keyboard command for mute in Skype, windows + F4
      Keyboard.press(KEY_F4);
    }
    delay(100);
    Keyboard.releaseAll();
    singleClick = false;
  }
  if ( i == 2 ) {
    mode ++;
    if (mode >= 4) {
      mode = 0;
    }
    if (mode == 0) {
      setPixels(StarleafColor[0], StarleafColor[1], StarleafColor[2]);
    }
    else if (mode == 1) {
      setPixels(TeamsColor[0], TeamsColor[1], TeamsColor[2]);
    }
    else if (mode == 2) {
      setPixels(WebexColor[0], WebexColor[1], WebexColor[2]);
    }
    else if (mode == 3) {
      setPixels(SkypeColor[0], SkypeColor[1], SkypeColor[2]);
    }
  }
}

void setPixels(int color0, int color1, int color2) {
  for (int pix = 0; pix < NUMPIXELS; pix++) {
    pixels.setPixelColor(pix, pixels.Color(color0, color1, color2));
  }
  pixels.show();
}
void StartUpColorTest2() { //define the base LED colour, USAGE: StartUpColorTest2();
  //Gradual Rainbow Routine
  rainbow(5); //wait time between colours
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
void rainbowMillis() {
  if ((millis() - lastRainbowMillis) > rainbowMillisDelay)
  {
    rainbowMillis_j ++;
    if (rainbowMillis_j >= 256)
      rainbowMillis_j = 0;

    uint i;
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + rainbowMillis_j) & 255));
    }
    pixels.show();
    lastRainbowMillis = millis();
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
void MutedLEDs() {
  if ((millis() - lastMutedBlink) > mutedBlinkDelay)
  {
    if (mutedBlinkOnOff) {
      if (mode == 0) {
        setPixels(StarleafColor[0], StarleafColor[1], StarleafColor[2]);
      }
      else if (mode == 1) {
        setPixels(TeamsColor[0], TeamsColor[1], TeamsColor[2]);
      }
      else if (mode == 2) {
        setPixels(WebexColor[0], WebexColor[1], WebexColor[2]);
      }
      else if (mode == 3) {
        setPixels(SkypeColor[0], SkypeColor[1], SkypeColor[2]);
      }
    }
    else {
      setPixels(0, 0, 0);
    }
    mutedBlinkOnOff = !mutedBlinkOnOff;
    lastMutedBlink = millis();
  }
}

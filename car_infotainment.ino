#include <OneWire.h>
#include <SoftwareSerial.h>
#include "helpers.h"

// in milliseconds
const int TempRefreshInterval = 2000;
const int AltIMURefreshInterval = 500;

//DS18S20 Signal pins
const int DS18S20_PinIn = 2;
const int DS18S20_PinOut = 3;

// screen TX and RX pins (RX pin is needed and yet not used)
const int ScreenTxPin = 10;
const int ScreenRxPin = 11;

// last screen symbol on the second line
char animationChars[] = "^<^>";
int loadingIndex = 0;

// temperature chip i/o
OneWire dsIn(DS18S20_PinIn);
OneWire dsOut(DS18S20_PinOut);

// screen output
SoftwareSerial screenSerial(ScreenTxPin, ScreenRxPin);

void setup(void) {
  screenSerial.begin(9600);

  clearScreen();
  changeScreenBrightness(140);
}

void loop(void) {
  // temperature is in Celsius
  float temperatureIn = getTemp(dsIn);
  float temperatureOut = getTemp(dsOut);

  // still don't have the AltIMU board, just write test values
  // on the screen
  char direction[] = "NE";

  // heading is in degrees
  float heading = 360;

  // altitude is in meters
  int altitude = 1000;

  refreshDisplayFirstLine(temperatureIn, temperatureOut);
  refreshDisplaySecondLine(direction, heading, altitude);

  waitAndUpdateAnimation(TempRefreshInterval, 4);
}

// ============================= DISPLAY =============================
void refreshDisplayFirstLine(float inTemp, float outTemp) {
  // first line: "I: 25.0 O: 35.0 "
  changeCursorPosition(0);

  screenSerial.write("I: ");

  String inValue = "";
  if (inTemp == -1000) {
    // the temperature sensor is malfunctioning
    inValue = "n/a ";
  } else {
    inValue = String(inTemp, 1);
  }

  writeValueRightToLeft(inValue, 7, 5);

  changeCursorPosition(7);
  screenSerial.write(" O:");

  String outValue = "";
  if (outTemp == -1000) {
    // the temperature sensor is malfunctioning
    outValue = "n/a ";
  } else {
    outValue = String(outTemp, 1);
  }

  writeValueRightToLeft(outValue, 15, 5);

  changeCursorPosition(15);
  screenSerial.write(" ");
}

void refreshDisplaySecondLine(char dir[], float heading, int altitude) {
  // second line: "N  358.0o 1000m^"
  changeCursorPosition(16);
  screenSerial.write(dir);

  String head = String(heading, 1);
  writeValueRightToLeft(head, 24, 5);

  changeCursorPosition(24);
  screenSerial.write("o");

  String alt = String(altitude);
  writeValueRightToLeft(alt, 30, 4);

  changeCursorPosition(30);
  screenSerial.write("m");
}

void waitAndUpdateAnimation(int totalTime, int timesToUpdateAnimation) {
  int dividedDelay = totalTime / timesToUpdateAnimation;
  for (int i = 0; i < timesToUpdateAnimation; i++) {
    updateAnimation();
    delay(dividedDelay);
  }
}

void updateAnimation() {
  // updating the animation symbol (last character on the second line)
  changeCursorPosition(31);

  screenSerial.write(animationChars[loadingIndex]);

  loadingIndex++;
  if (loadingIndex == sizeof(animationChars) - 1) {
    loadingIndex = 0;
  }
}

void writeValueRightToLeft(String val, int pos, int maxLen) {
  for (int i = val.length(), j = 0, len = 0; len <= maxLen; i--, j++, len++) {
    changeCursorPosition(pos - j);

    if (i >= 0) {
      screenSerial.write(val[i]);
    } else {
      screenSerial.write(" ");
    }
  }
}

void changeCursorPosition(int pos) {
  // second line begins from 64
  if (pos > 15) {
    pos = (pos - 16) + 64;
  }

  screenSerial.write(0xFE);
  screenSerial.write(pos + 128);
}

void clearScreen() {
  screenSerial.write(0xFE);
  screenSerial.write(0x01);
}

void changeScreenBrightness(int brightness) {
  screenSerial.write(0x7C);
  screenSerial.write(brightness);
}

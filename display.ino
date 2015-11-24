void refreshDisplayFirstLine(float inTemp, float outTemp) {
  // first line: "I: 25.0 O: 35.0 "
  changeCursorPosition(0);

  screenSerial.write("I: ");

  String inValue = "";
  if (inTemp == -127.00) {
    // the temperature sensor is malfunctioning
    inValue = "n/a ";
  } else {
    inValue = String(inTemp, 1);
  }

  writeValueRightToLeft(inValue, 7, 5);

  changeCursorPosition(7);
  screenSerial.write(" O:");

  String outValue = "";
  if (outTemp == -127.00) {
    // the temperature sensor is malfunctioning
    outValue = "n/a ";
  } else {
    outValue = String(outTemp, 1);
  }

  writeValueRightToLeft(outValue, 15, 5);

  changeCursorPosition(15);
  if (outTemp <= 4 && outTemp != -127.00) {
    screenSerial.write("*");
  } else {
    screenSerial.write(" ");
  }
}

void refreshDisplaySecondLineCompass(char dir[], float heading) {
  // second line: "N  358.0o 1000m^"
  changeCursorPosition(16);
  screenSerial.write(dir);

  String head = "";
  if (heading == -1.0) {
    head = "n/a ";
  } else {
    head = String(heading, 1);
  }
  writeValueRightToLeft(head, 24, 5);

  changeCursorPosition(24);
  screenSerial.write("o");
}

void refreshDisplaySecondLineAltitude(int altitude) {
  String alt = "";
  if (altitude == -1) {
    alt = "n/a ";
  } else {
    alt = String(altitude);
  }
  writeValueRightToLeft(alt, 30, 4);

  changeCursorPosition(30);
  screenSerial.write("m");
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

//void checkDimButton() {
//  if (digitalRead(ScreenDimBtnPin) == HIGH) {
//    changeScreenBrightness(ScreenDimOn);
//  } else {
//    changeScreenBrightness(ScreenDimOff);
//  }
//}

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

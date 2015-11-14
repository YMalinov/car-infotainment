#include <OneWire.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LPS_Pressure.h>
#include <LSM303_Magnet.h>

// in milliseconds
const int TempRefreshInterval = 2000;
const int AltIMURefreshInterval = 500;

//DS18S20 Signal pins
const int DS18S20_PinIn = 2;
const int DS18S20_PinOut = 3;

// screen TX and RX pins (RX pin is needed and yet not used)
const int ScreenTxPin = 10;
const int ScreenRxPin = 11;

// screen dim button pin
const int ScreenDimBtnPin = 7;

// screen dim values
const int ScreenDimOn = 140;
const int ScreenDimOff = 150;

// AltIMU specific settings
LSM303::vector<int16_t> CompassMin = LSM303::vector<int16_t> {  +858,   +324,  -2232};
LSM303::vector<int16_t> CompassMax = LSM303::vector<int16_t> {  +868,   +340,  -2216};


// AltIMU sensors
LPS pressureSensor;
LSM303 magneticSensor;

// AltIMU sensors detection
boolean pressureSensorDetected = false;
boolean magneticSensorDetected = false;

// last screen symbol on the second line
char animationChars[] = "^<^>";
int loadingIndex = 0;

// temperature chip i/o
OneWire tempIn(DS18S20_PinIn);
OneWire tempOut(DS18S20_PinOut);

// screen output
SoftwareSerial screenSerial(ScreenTxPin, ScreenRxPin);

void setup(void) {
  screenSerial.begin(9600);

  clearScreen();
  //  changeScreenBrightness(140);

  Wire.begin();

  if (pressureSensor.init()) {
    pressureSensorDetected = true;
    pressureSensor.enableDefault();
  }

  if (magneticSensor.init()) {
    magneticSensorDetected = true;
    magneticSensor.enableDefault();

    magneticSensor.m_min = CompassMin;
    magneticSensor.m_max = CompassMax;
  }

  pinMode(ScreenDimBtnPin, INPUT);
}

void loop(void) {


  // temperature is in Celsius
  float temperatureIn = getTemp(tempIn);
  float temperatureOut = getTemp(tempOut);

  refreshDisplayFirstLine(temperatureIn, temperatureOut);

  // heading is in degrees
  float heading = -1.0;
  if (magneticSensorDetected) {
    magneticSensor.read();
    heading = magneticSensor.heading();
  }

  char *direction;
  getHeadingAsString(heading, &direction);

  // altitude is in meters
  int altitude = -1;
  if (pressureSensorDetected) {
    float pressure = pressureSensor.readPressureMillibars();
    float altitudeAsFloat = pressureSensor.pressureToAltitudeMeters(pressure);

    altitude = (int)altitudeAsFloat;
  }

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

  String head = "";
  if (heading == -1.0) {
    head = "n/a ";
  } else {
    head = String(heading, 1);
  }
  writeValueRightToLeft(head, 24, 5);

  changeCursorPosition(24);
  screenSerial.write("o");

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

void waitAndUpdateAnimation(int totalTime, int timesToUpdateAnimation) {
  int dividedDelay = totalTime / timesToUpdateAnimation;
  for (int i = 0; i < timesToUpdateAnimation; i++) {
    updateAnimation();
    checkDimButton();
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

void checkDimButton() {
  if (digitalRead(ScreenDimBtnPin) == HIGH) {
    changeScreenBrightness(ScreenDimOn);
  } else {
    changeScreenBrightness(ScreenDimOff);
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

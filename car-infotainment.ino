#include <OneWire.h> // used by temperature sensors
#include <DallasTemperature.h> // used by temperature sensors
#include <Wire.h> // used by lps pressure and lsm303 sensors
#include <SoftwareSerial.h> // used by display
#include <LPS_Pressure.h>
#include <LSM303_Magnet.h>

// in milliseconds
#define TEMP_REFRESH_INTERVAL       2000
#define ALTIMETER_REFRESH_INTERVAL  5000
#define COMPASS_REFRESH_INTERVAL     300
#define ANIMATION_REFRESH_INTERVAL   500
#define LOOP_WAIT_INTERVAL            50

// temperature sensors OneWire signal pin
#define TEMP_SENSORS_PIN 12

// temperature sensors resolution (9 to 12 inclusive)
#define TEMP_RESOLUTION 10

// screen TX and RX pins (TX pin is needed and yet not used)
#define SCREEN_TX_PIN 10
#define SCREEN_RX_PIN 11
#define SCREEN_BAUDRATE 9600
#define SCREEN_BRIGHTNESS 140

// temperature chip i/o
OneWire oneWire(TEMP_SENSORS_PIN);
DallasTemperature tempSensors(&oneWire);

//DS18S20 hardware addresses
DeviceAddress InTemp =  { 0x28, 0x4E, 0x76, 0x74, 0x06, 0x00, 0x00, 0xFE };
DeviceAddress OutTemp = { 0x28, 0xFF, 0x7E, 0x74, 0x06, 0x00, 0x00, 0xF0 };

// screen dim button pin
//const int ScreenDimBtnPin = 7;

// screen dim values
//const int ScreenDimOn = 140;
//const int ScreenDimOff = 150;

// AltIMU specific settings
LSM303::vector<int16_t> CompassMin = LSM303::vector<int16_t> {  +846,   +400,  -2451};
LSM303::vector<int16_t> CompassMax = LSM303::vector<int16_t> {  +869,   +423,  -2409};
// LSM303::vector<int16_t> CompassMin = (LSM303::vector<int16_t>){-32767, -32767, -32767};
// LSM303::vector<int16_t> CompassMax = (LSM303::vector<int16_t>){+32767, +32767, +32767};

// AltIMU sensors
LPS pressureSensor;
LSM303 magneticSensor;

// AltIMU sensors detection
boolean pressureSensorDetected = false;
boolean magneticSensorDetected = false;

// last screen symbol on the second line
char animationChars[] = "^<^>";
int loadingIndex = 0;

// screen output
SoftwareSerial screenSerial(SCREEN_TX_PIN, SCREEN_RX_PIN);

// async update milliseconds
unsigned long lastTemperatureMillis = millis();
unsigned long lastAltimeterMillis = millis();
unsigned long lastCompassMillis = millis();
unsigned long lastAnimationMillis = millis();

void setup(void) {
  // for debugging purposes
//    Serial.begin(9600);

  // setting up screen
  screenSerial.begin(SCREEN_BAUDRATE);
  clearScreen();
  changeScreenBrightness(SCREEN_BRIGHTNESS);

  // setting up temperature sensors
  tempSensors.begin();

  // setting the resolution of the temperature sensors
  tempSensors.setResolution(InTemp, TEMP_RESOLUTION);
  tempSensors.setResolution(OutTemp, TEMP_RESOLUTION);

  // setting up Wire instance for pressure and magnetic
  // sensor
  Wire.begin();

  // setting up atmospheric pressure sensor
  if (pressureSensor.init()) {
    pressureSensorDetected = true;
    pressureSensor.enableDefault();
  }

  // setting up compass
  if (magneticSensor.init()) {
    magneticSensorDetected = true;
    magneticSensor.enableDefault();

    magneticSensor.m_min = CompassMin;
    magneticSensor.m_max = CompassMax;
  }
}

void loop(void) {
  checkIfShouldUpdateTemperatureValues();
  checkIfShouldUpdateCompassValues();
  checkIfShouldUpdateAltimeterValues();
  checkIfShouldUpdateAnimation();

  delay(LOOP_WAIT_INTERVAL);
}

void checkIfShouldUpdateTemperatureValues() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastTemperatureMillis >= TEMP_REFRESH_INTERVAL) {
    lastTemperatureMillis = currentMillis;
    
    // get temperature values from sensors
    tempSensors.requestTemperatures();

    // temperature is in Celsius
    float temperatureIn = tempSensors.getTempC(InTemp);
    float temperatureOut = tempSensors.getTempC(OutTemp);

    refreshDisplayFirstLine(temperatureIn, temperatureOut);
  }
}

void checkIfShouldUpdateCompassValues() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastCompassMillis >= COMPASS_REFRESH_INTERVAL) {
    lastCompassMillis = currentMillis;
    
    // heading is in degrees
    float heading = -1.0;
    if (magneticSensorDetected) {
      magneticSensor.read();
      heading = magneticSensor.heading();
    }

    char *direction;
    getHeadingAsString(heading, &direction);

    refreshDisplaySecondLineCompass(direction, heading);
  }
}

void checkIfShouldUpdateAltimeterValues() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastAltimeterMillis >= ALTIMETER_REFRESH_INTERVAL) {
    lastAltimeterMillis = currentMillis;
    
    // altitude is in meters
    int altitude = -1;
    if (pressureSensorDetected) {
      float pressure = pressureSensor.readPressureMillibars();
      float altitudeAsFloat = pressureSensor.pressureToAltitudeMeters(pressure);

      altitude = (int)altitudeAsFloat;
    }

    refreshDisplaySecondLineAltitude(altitude);
  }
}

void checkIfShouldUpdateAnimation() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastAnimationMillis >= ANIMATION_REFRESH_INTERVAL) {
    lastAnimationMillis = currentMillis;
    
    updateAnimation();
  }
}

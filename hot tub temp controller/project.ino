#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* 
*  ######################
*  #  Pins definitions  #
*  ######################
*/

// screen
#define sclk 13
#define mosi 11
#define rst 12
#define dc 10
#define cs 9

// sensors
#define waterPin 8
#define tempPin 7

// alarm
#define alarmPin 6

// output
#define mosfetPin 5

// dip switch
#define b0 4
#define b1 3
#define b2 2

/* 
*  ##########
*  #  OLED  #
*  ##########
*/

// image dimensions
#define imageWidth 128
#define imageHeight 39

const unsigned char logo[] PROGMEM = {
  //128x39px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xfe, 0x7f, 0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xfe, 0x7f, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x3e, 0x7f, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x3e, 0x7f, 0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x3e, 0x7f, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x0e, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x1e, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xcc, 0x3e, 0x00, 0x73, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfc,
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xcf, 0xfe, 0x7f, 0xf3, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe,
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xfe, 0x7f, 0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xcf, 0xcc, 0x3f, 0x7e, 0xfc, 0xfc, 0x7e, 0x30, 0xcf, 0xcf, 0x9f, 0x1f, 0x31, 0x99, 0xf1, 0xf3,
  0xcf, 0xcc, 0x3f, 0x7e, 0xfc, 0xce, 0xe7, 0x39, 0xcf, 0xdf, 0xbf, 0x3f, 0x39, 0x9b, 0xf3, 0xf3,
  0xcc, 0x0c, 0x30, 0x18, 0x30, 0xce, 0xc3, 0x3f, 0xcc, 0x1c, 0x38, 0x63, 0x3f, 0x9b, 0x86, 0x33,
  0xcf, 0x8c, 0x3e, 0x18, 0x30, 0xfc, 0xc3, 0x36, 0xcf, 0x9c, 0x38, 0x7f, 0x3f, 0x9b, 0x87, 0xf3,
  0xcf, 0x8c, 0x3e, 0x18, 0x30, 0xf8, 0xc3, 0x30, 0xcf, 0x9c, 0x38, 0x7f, 0x37, 0x9b, 0x87, 0xf3,
  0xcc, 0x0c, 0x30, 0x18, 0x30, 0xdc, 0xe7, 0x30, 0xcc, 0x1c, 0x38, 0x63, 0x33, 0x9b, 0x86, 0x33,
  0xcf, 0xcf, 0xbf, 0x18, 0x30, 0xce, 0x7e, 0x30, 0xcf, 0xcf, 0x9f, 0x63, 0x31, 0x99, 0xf6, 0x33,
  0xcf, 0xcf, 0xbf, 0x18, 0x30, 0xc6, 0x3c, 0x30, 0xcf, 0xc7, 0x8f, 0x63, 0x31, 0x98, 0xf6, 0x33,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e,
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8
};

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

//Text definition
#define OLED_Backround_Color BLACK
#define OLED_Text_Color WHITE
#define sizeW 1
#define sizeH 3

// Create an instance of the SSD1331 display driver
Adafruit_SSD1351 display = Adafruit_SSD1351(128, 128, &SPI, cs, dc, rst);

/* 
*  ###############
*  #  Variables  #
*  ###############
*/

// dip switch
String bitString;

// temperature
#define MIN_TEMP 24
#define MAX_TEMP 36
#define DELTA_TEMP 2
#define ERROR_TEMP -127
int short select[] = { 0, MIN_TEMP, 0, 0, 0, 0, 0, MAX_TEMP };

// water sensor
bool isWaterPresent;
short selectedTemp;

// temp sensor
OneWire oneWire(tempPin);
DallasTemperature sensors(&oneWire);
float currentTemp;

/* sensors status
*   0 = sensors ok
*   1 = temp sensor missing/broken
*   2 = water missing
*   3 = 1 & 2 at the same time
*/
short sensorsStatus;

/* system status
*   false = off
*   true = on
*/
bool systemStatus;

/* alarm status
*   false = off
*   true = on
*/
bool alarmStatus;

/* 
*  ###########
*  #  Setup  #
*  ###########
*/
void setup() {
  // Serial monitor for debug
  Serial.begin(9600);

  // Initialize the display
  display.begin();
  display.fillScreen(OLED_Backround_Color);
  display.drawBitmap(0, 64 - 39 / 2, logo, imageWidth, imageHeight, WHITE);
  delay(2500);
  display.fillScreen(OLED_Backround_Color);
  display.setTextColor(OLED_Text_Color);
  display.setTextSize(sizeW, sizeH);

  // input/output
  pinMode(b0, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(waterPin, INPUT);
  pinMode(mosfetPin, OUTPUT);

  // temp sensor initialization
  sensors.begin();

  // status reset
  applyStatus(&systemStatus, false, mosfetPin);
  applyStatus(&alarmStatus, false, alarmPin);

  // temp options
  short inc = (MAX_TEMP - MIN_TEMP) / 6;
  for (short i = 2; i <= 6; i++) {
    select[i] = select[i - 1] + inc;
  }
}

/* 
*  ##########
*  #  Loop  #
*  ##########
*/
void loop() {
  // read and check sensors
  checkSensors();

  // alarm check
  while (sensorsStatus > 0) {
    applyStatus(&systemStatus, false, mosfetPin);
    applyStatus(&alarmStatus, true, alarmPin);
    updateScreen();
    Serial.println(sensorsStatus);
    delay(100);
    checkSensors();
  }

  // deactive alarm if active
  if(alarmStatus == true) 
    applyStatus(&alarmStatus, false, alarmPin);

  // dip switch read
  bitString = String(!digitalRead(b2)) + String(!digitalRead(b1)) + String(!digitalRead(b0));
  selectedTemp = select[strtol(bitString.c_str(), NULL, 2)];

  Serial.println(currentTemp);
  Serial.println(selectedTemp);

  // water temp is low, system is off
  if (currentTemp <= (selectedTemp - DELTA_TEMP) && !systemStatus)
    applyStatus(&systemStatus, true, mosfetPin);

  // water temp is high, system is on
  if (currentTemp >= selectedTemp && systemStatus)
    applyStatus(&systemStatus, false, mosfetPin);

  updateScreen();
  //delay(10);
}

/* 
*  ##################
*  #  checkSensors  #
*  ##################
*/
void checkSensors() {
  sensorsStatus = 0;

  // temp read
  sensors.requestTemperatures();
  currentTemp = sensors.getTempCByIndex(0);

  // water read
  isWaterPresent = !digitalRead(waterPin);

  // update of sensorsStatus if needed
  if (currentTemp == ERROR_TEMP) sensorsStatus += 1;
  if (!isWaterPresent) sensorsStatus += 2;
}

/* 
*  #################
*  #  applyStatus  #
*  #################
*/
void applyStatus(bool *variable, bool status, int pin) {

  // turn on or off mosfet
  if (status) {
    analogWrite(pin, 255);
  } else {
    analogWrite(pin, 0);
  }

  // set status
  *variable = status;
}

/* 
*  ##################
*  #  updateScreen  #
*  ##################
*/
void updateScreen() {
  display.fillScreen(OLED_Backround_Color);
  display.setCursor(0, 0);
  display.print("Temp impostata: ");
  display.println(selectedTemp);
  display.print("Temp attuale: ");
  display.println(currentTemp);
  display.print("Stato sistema: ");
  display.println(systemStatus);
  display.print("Codice errore: ");
  display.println(sensorsStatus);
}
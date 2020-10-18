// Author: Nathan Merrill

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "RTClib.h"

const int threePosition1A = A0;
const int threePosition1B = A1;
const int threePosition2A = A2;
const int threePosition2B = A3;

const int twoPosition = 2;

const int momentary1 = 4;
const int momentary2 = 5;
const int momentary3 = 7;
const int momentary4 = 8;

const int lightSwitch = 12;

//const int rtcInterrupt = 6;

const int buzzer = 9;

const int dimmerZeroCrossing = 2;
const int dimmer = 3;

int alarm1Hour = 8;
int alarm1Minute = 0;
int alarm2Hour = 9;
int alarm2Minute = 15;

//int previousMinute;
//int previousThreePosition1;
//int previousThreePosition2;
//boolean previousTwoPosition;
//boolean previousLightSwitch;
//boolean previousMomentary1;
//boolean previousMomentary2;
//boolean previousMomentary3;
//boolean previousMomentary4;

Adafruit_7segment matrix = Adafruit_7segment();
RTC_DS3231 rtc;

void error(String message) {
  Serial.println("ERROR: " + message);
  while (true) {
    delay(1000);
  }
}

boolean fixPullUp(boolean val) {
  return !val;
}

int readThreePosition(int number) {
  bool a = digitalRead(threePosition1A);
  bool b = digitalRead(threePosition1B);
  if (number == 2) {
    a = digitalRead(threePosition2A);
    b = digitalRead(threePosition2B);
  }
  if (b && !a) {
    return 0;
  } else if (a && b) {
    return 1;
  } else if (a && !b) {
    return 2;
  }
  error("Three position switch broken");
}

void onMinuteChange(DateTime now) {
  
}

void onThreePosition1Change(int position) {
  
}

void setup() {
  Serial.begin(9600);
  Serial.println("Program start");

  // Set up pins
  pinMode(threePosition1A, INPUT_PULLUP);
  pinMode(threePosition1B, INPUT_PULLUP);
  pinMode(threePosition2A, INPUT_PULLUP);
  pinMode(threePosition2B, INPUT_PULLUP);

  pinMode(twoPosition, INPUT_PULLUP);

  pinMode(momentary1, INPUT_PULLUP);
  pinMode(momentary2, INPUT_PULLUP);
  pinMode(momentary3, INPUT_PULLUP);
  pinMode(momentary4, INPUT_PULLUP);
  
  pinMode(lightSwitch, INPUT_PULLUP);
  
//  pinMode(rtcInterrupt, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);

//  pinMode(dimmerZeroCrossing, INPUT);
  pinMode(dimmer, OUTPUT);


  // Set up display
  matrix.begin(0x70);
  matrix.setBrightness(0);
//  matrix.print(1234);
  matrix.drawColon(true);
  matrix.writeDisplay();


  // Set up RTC
  if (!rtc.begin()) {
    error("Couldn't find RTC");
  }
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting it now");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(10)); // Sets RTC to current system time
  }
}

void loop(){
  DateTime now = rtc.now();

  int minute = now.minute();
  if (previousMinute != minute) {
    previousMinute = minute;
    Serial.println(minute);
    onMinuteChange(now);
  }

  int threePosition1 = readThreePosition(1);
  if (previousThreePosition1 != threePosition1) {
    previousThreePosition1 = threePosition1;
    Serial.println(threePosition1);
    onThreePosition1Change(threePosition1);
  }
//  delay(1000);
}

// Author: Nathan Merrill

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "RTClib.h"
#include <RBDdimmer.h>

const int threePosition1A = A0;
const int threePosition1B = A1;
const int threePosition2A = A2;
const int threePosition2B = A3;

const int twoPosition = 6;

const int momentary1 = 4;
const int momentary2 = 5;
const int momentary3 = 7;
const int momentary4 = 8;

const int lightSwitch = 12;

const int buzzer = 9;

const int dimmerPin = 3;

const int relay = 13;

int alarm1Time = 800;
boolean alarm1PM = false;
int alarm2Time = 915;
boolean alarm2PM = false;

Adafruit_7segment matrix = Adafruit_7segment();
RTC_DS3231 rtc;

class Listener {
  private:
    int (*getValue)();
    void (*onActivate)(int val);
    int previousValue;
  public:
    Listener(int (*getValueArg)(), void (*onActivateArg)(int val)) {
      getValue = getValueArg;
      onActivate = onActivateArg;
    }

    void check() {
      int val = getValue();
      if (previousValue != val) {
        previousValue = val;
        Serial.print("Value changed: ");
        Serial.println(val);
        onActivate(val);
      }
    }
};

void error(String message) {
  Serial.println("ERROR: " + message + " *******************************************************");
  while (true) {
    delay(1000);
  }
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

void displayWrite(int text, boolean lastPeriod = false) {
  if (!digitalRead(twoPosition)) {
    matrix.print(text);
    matrix.drawColon(true);
    Serial.print("aaa: ");  
    int value = String(text).substring(String(text).length()-1, String(text).length()).toInt();
    Serial.print("last digit: ");
    Serial.println(value);
    matrix.writeDigitNum(4, value, lastPeriod);
    matrix.writeDisplay();
  }

}

void displayTime() {
  DateTime now = rtc.now();
  displayWrite(now.twelveHour() * 100 + now.minute(), now.isPM());
//  char out[5];
//  sprintf(out, "%.2d%.2d", now.hour(), now.minute());
//  displayWrite(atoi(out));
}

void displayAlarm(int number) {
  Serial.print("DISPLAYING ALARM ");
  Serial.println(number);
//  int hour = alarm1Hour;
//  int minute = alarm1Minute;
//  if (number == 2) {
//    hour = alarm2Hour;
//    minute = alarm2Minute;
//  }

//  Serial.print(hour);
//  Serial.println(minute);
  
//  if (String(hour).length() == 1) {
//    Serial.println("adding padding hour");
//    sprintf(hour, "0%.1d", hour);
//  }
//  if (String(minute).length() == 1) {
//    Serial.println("adding padding minute");
//    sprintf(minute, " %.1d", minute);
//  }

//  Serial.print(hour);
//  Serial.println(minute);

//  char out[5];
//  sprintf(out, "%.2d%.2d", hour, minute);
//  Serial.println(out);
//  displayWrite(atoi(out));
  if (number == 1) {
    displayWrite(alarm1Time, alarm1PM);
  } else if (number == 2) {
    Serial.print("alarm2aaaaa: ");
    Serial.print(alarm2Time);
    Serial.print(" ");
    Serial.println(alarm2PM);
    displayWrite(alarm2Time, alarm2PM);
  } else {
    error("INVALID ALARM DISPLAY NUMBER");
  }
}

void onMinuteChange(int minute) {
  if (readThreePosition(1) == 0) {
    displayTime();
  }
}
  
void onThreePosition1Change(int position) {
  Serial.print("THREE POSITION 1 CHANGE: ");
  Serial.println(position);
  if (position == 2) {
    int threePosition2 = readThreePosition(2);
    if (threePosition2 == 0) {
      displayWrite(8888);
    } else {
      Serial.print("DISPLAYING ALARM: ");
      Serial.println(threePosition2);
      displayAlarm(threePosition2);
    }
  } else {
    displayTime();
  }
}

void onThreePosition2Change(int position) {
  Serial.print("THREE POSITION 2 CHANGE: ");
  Serial.println(position);
//  if (readThreePosition(1) == 2) {
//    displayAlarm(position);
//  }
  onThreePosition1Change(readThreePosition(1));
}

void onTwoPositionChange(int position) {
  Serial.println("TWO POSITION CHANGE");
  if (position == 1) {
    matrix.clear();
    matrix.writeDisplay();
  } else {
    onThreePosition1Change(readThreePosition(1));
  }
}

void onMomentaryChange(int number, int position) {
  if (!position) {
    int threePosition1 = readThreePosition(1);
    if (threePosition1 == 1) { // Adjusting time
      Serial.println("CHANGING TIME");
      DateTime now = rtc.now();
      if (number == 1) {
        rtc.adjust(now + TimeSpan(3600));
      } else if (number == 2) {
        rtc.adjust(now - TimeSpan(3600));
      } else if (number == 3) {
        rtc.adjust(now + TimeSpan(60));
      } else if (number == 4) {
        rtc.adjust(now - TimeSpan(60));
      } else {
        error("INVALID MOMENTARY NUMBER");
      }
      displayTime();
    } else if (threePosition1 == 2) { // Adjusting alarm
      int threePosition2 = readThreePosition(2);
      if (threePosition2 == 1) {
        Serial.println("CHANGING ALARM 1");
        Serial.println(alarm1Time);
        Serial.println(alarm1PM);
        int alarm1Minute = String(alarm1Time).substring(1, 3).toInt();
        int alarm1Hour = String(alarm1Time).substring(0, 1).toInt();
        if (String(alarm1Time).length() == 4) {
          Serial.println("4 digit time");
          alarm1Minute = String(alarm1Time).substring(2, 4).toInt();
          alarm1Hour = String(alarm1Time).substring(0, 2).toInt();
        }
        Serial.print("Hour: ");
        Serial.println(alarm1Hour);
        Serial.print("Minute: ");
        Serial.println(alarm1Minute);
        if (number == 1) {
          alarm1Time += 100;
          if (alarm1Time >= 1259) {
            alarm1Time -= 1200;
          }
          if (alarm1Hour == 11) {
            alarm1PM = !alarm1PM;
          }
        } else if (number == 2) {
          alarm1Time -= 100;
          if (alarm1Time < 100) {
            alarm1Time += 1200;
          }
          if (alarm1Hour == 12) {
            alarm1PM = !alarm1PM;
          }
        } else if (number == 3) {
          if (alarm1Minute == 59) {
            alarm1Time -= 59;
          } else {
            alarm1Time += 1;
          }
        } else if (number == 4) {
          if (alarm1Minute == 0) {
            alarm1Time += 59;
          } else {
            alarm1Time -= 1;
          }
        } else {
          error("INVALID MOMENTARY NUMBER");
        }
        displayAlarm(1);
      } else if (threePosition2 == 2) {
        Serial.println("CHANGING ALARM 2");
//        Serial.println(alarm1Time);
//        Serial.println(alarm1PM);
        int alarm2Minute = String(alarm2Time).substring(1, 3).toInt();
        int alarm2Hour = String(alarm2Time).substring(0, 1).toInt();
        if (String(alarm2Time).length() == 4) {
//          Serial.println("4 digit time");
          alarm2Minute = String(alarm2Time).substring(2, 4).toInt();
          alarm2Hour = String(alarm2Time).substring(0, 2).toInt();
        }
//        Serial.print("Hour: ");
//        Serial.println(alarm1Hour);
//        Serial.print("Minute: ");
//        Serial.println(alarm1Minute);
        if (number == 1) {
          alarm2Time += 100;
          if (alarm2Time >= 1259) {
            alarm2Time -= 1200;
          }
          if (alarm2Hour == 11) {
            alarm2PM = !alarm2PM;
          }
        } else if (number == 2) {
          alarm2Time -= 100;
          if (alarm2Time < 100) {
            alarm2Time += 1200;
          }
          if (alarm2Hour == 12) {
            alarm2PM = !alarm2PM;
          }
        } else if (number == 3) {
          if (alarm2Minute == 59) {
            alarm2Time -= 59;
          } else {
            alarm2Time += 1;
          }
        } else if (number == 4) {
          if (alarm2Minute == 0) {
            alarm2Time += 59;
          } else {
            alarm2Time -= 1;
          }
        } else {
          error("INVALID MOMENTARY NUMBER");
        }
        displayAlarm(2);
      }
    }
  }
}

void onMomentary1Change(int position) {
  onMomentaryChange(1, position);
}

void onMomentary2Change(int position) {
  onMomentaryChange(2, position);
}

void onMomentary3Change(int position) {
  onMomentaryChange(3, position);
}

void onMomentary4Change(int position) {
  onMomentaryChange(4, position);
}

void onLightSwitchChange(int position) {
  Serial.println("LIGHT SWITCH CHANGE");
}

//Listener minuteListener([rtc]() -> int {return rtc.now().minute();}, onMinuteChange);
Listener listeners[] = {
  Listener([]() -> int {return rtc.now().minute();}, onMinuteChange),
  Listener([]() -> int {return readThreePosition(1);}, onThreePosition1Change),
  Listener([]() -> int {return readThreePosition(2);}, onThreePosition2Change),
  Listener([]() -> int {return digitalRead(twoPosition);}, onTwoPositionChange),
  Listener([]() -> int {return digitalRead(momentary1);}, onMomentary1Change),
  Listener([]() -> int {return digitalRead(momentary2);}, onMomentary2Change),
  Listener([]() -> int {return digitalRead(momentary3);}, onMomentary3Change),
  Listener([]() -> int {return digitalRead(momentary4);}, onMomentary4Change),
  Listener([]() -> int {return digitalRead(lightSwitch);}, onLightSwitchChange),
};

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
  pinMode(dimmerPin, OUTPUT);

  // Set up display
  matrix.begin(0x70);
  matrix.setBrightness(0);
//  matrix.print(1234);
//  matrix.drawColon(true);
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
//  minuteListener.check();
  for (int i = 0; i < (sizeof(listeners)/sizeof(listeners[0])); i++) {
    listeners[i].check();
  }
}

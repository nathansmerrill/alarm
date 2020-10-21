// Author: Nathan Merrill

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "RTClib.h"

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

void displayWrite(int text) {
  matrix.print(text);
  matrix.drawColon(true);
  matrix.writeDisplay();
}

void displayTime() {
  DateTime now = rtc.now();
  char out[5];
  sprintf(out, "%.2d%.2d", now.hour(), now.minute());
  displayWrite(atoi(out));
}

void displayAlarm(int number) {
  Serial.print("DISPLAYING ALARM ");
  Serial.println(number);
  int hour = alarm1Hour;
  int minute = alarm1Minute;
  if (number == 2) {
    hour = alarm2Hour;
    minute = alarm2Minute;
  }

  Serial.print(hour);
  Serial.println(minute);
  
//  if (String(hour).length() == 1) {
//    Serial.println("adding padding hour");
//    sprintf(hour, "0%.1d", hour);
//  }
//  if (String(minute).length() == 1) {
//    Serial.println("adding padding minute");
//    sprintf(minute, " %.1d", minute);
//  }

  Serial.print(hour);
  Serial.println(minute);

  char out[5];
  sprintf(out, "%.2d%.2d", hour, minute);
  Serial.println(out);
  displayWrite(atoi(out));
}

void onMinuteChange(int minute) {
  if (readThreePosition(1) == 0) {
    displayTime();
  }
}
  
void onThreePosition1Change(int position) {
  DateTime now = rtc.now();
  if (position == 2) {
    int threePosition2 = readThreePosition(2);
    if (threePosition2 == 0) {
      displayWrite(8888);
    } else {
      displayAlarm(threePosition2);
    }
  } else {
    displayTime();
  }
}

void onThreePosition2Change(int position) {
//  if (readThreePosition(1) == 2) {
//    displayAlarm(position);
//  }
  onThreePosition1Change(readThreePosition(1));
}

void onTwoPositionChange(int position) {
  Serial.println("TWO POSITION CHANGE");
}

void onMomentary1Change(int position) {
//  Serial.println("Momentary 1 change");
  if (!position) {
    int threePosition1 = readThreePosition(1);
    if (threePosition1 == 1) {
      Serial.println("INCREASING TIME HOUR");
    } else if (threePosition1 == 2) {
      int threePosition2 = readThreePosition(2);
      if (threePosition2 == 1) {
        Serial.println("increasing alarm 1 hour");
        alarm1Hour++;
        displayAlarm(1);
      } else if (threePosition2 == 2) {
        Serial.println("increasing alarm 2 hour");
        alarm2Hour++;
        displayAlarm(2);
      }
    }
  }
}

void onMomentary2Change(int position) {
  if (!position) {
    int threePosition1 = readThreePosition(1);
    if (threePosition1 == 1) {
      Serial.println("DESCREASING TIME HOUR");
    } else if (threePosition1 == 2) {
      int threePosition2 = readThreePosition(2);
      if (threePosition2 == 1) {
        Serial.println("descreasing alarm 1 hour");
        alarm1Hour--;
        displayAlarm(1);
      } else if (threePosition2 == 2) {
        Serial.println("descreasing alarm 2 hour");
        alarm2Hour--;
        displayAlarm(2);
      }
    }
  }
}

void onMomentary3Change(int position) {
  if (!position) {
    int threePosition1 = readThreePosition(1);
    if (threePosition1 == 1) {
      Serial.println("INCREASING TIME MINUTE");
    } else if (threePosition1 == 2) {
      int threePosition2 = readThreePosition(2);
      if (threePosition2 == 1) {
        Serial.println("increasing alarm 1 minute");
        alarm1Minute++;
        displayAlarm(1);
      } else if (threePosition2 == 2) {
        Serial.println("increasing alarm 2 minute");
        alarm2Minute++;
        displayAlarm(2);
      }
    }
  }
}

void onMomentary4Change(int position) {
  if (!position) {
    int threePosition1 = readThreePosition(1);
    if (threePosition1 == 1) {
      Serial.println("DECREASING TIME HOUR");
    } else if (threePosition1 == 2) {
      int threePosition2 = readThreePosition(2);
      if (threePosition2 == 1) {
        Serial.println("decreasing alarm 1 minute");
        alarm1Minute--;
        displayAlarm(1);
      } else if (threePosition2 == 2) {
        Serial.println("decreasing alarm 2 minute");
        alarm2Minute--;
        displayAlarm(2);
      }
    }
  }
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
  pinMode(dimmer, OUTPUT);

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

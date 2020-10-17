// Author: Nathan Merrill

const int threePosition1A = A0;
const int threePosition1B = A1;
const int threePosition2A = A2;
const int threePosition2B = A3;

const int twoPosition = 2;

const int momentary1 = 4;
const int momentary2 = 5;
const int momentary3 = 7;
const int momentary4 = 8;

const int buzzer = 9;

const int lightSwitch = 12;


void setup() {
  Serial.begin(9600);
  Serial.println("Program start");
  
  pinMode(threePosition1A, INPUT_PULLUP);
  pinMode(threePosition1B, INPUT_PULLUP);
  pinMode(threePosition2A, INPUT_PULLUP);
  pinMode(threePosition2B, INPUT_PULLUP);

  pinMode(twoPosition, INPUT_PULLUP);

  pinMode(momentary1, INPUT_PULLUP);
  pinMode(momentary2, INPUT_PULLUP);
  pinMode(momentary3, INPUT_PULLUP);
  pinMode(momentary4, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);

  pinMode(lightSwitch, INPUT_PULLUP);

}

void loop() {
}

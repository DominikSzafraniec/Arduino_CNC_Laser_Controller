/*
 Name:		Arduino_CNC_Laser_Controller.ino
 Created:	17.03.2021 22:24:43
 Author:	Dominik
*/
#include <Arduino.h>

bool on = false;
bool cool = false;
String s;
int numOfSteps = 1;
static byte endSwitchesState = 0x00;

//endswitches pinout
static const byte xPositivePin = 38;
static const byte xNegativePin = 39;
static const byte yPositivePin = 34;
static const byte yNegativePin = 35;
static const byte zPositivePin = 30;
static const byte zNegativePin = 31;
//endswitches Bit
static const byte xPositiveBit = 0;
static const byte xNegativeBit = 1;
static const byte yPositiveBit = 2;
static const byte yNegativeBit = 3;
static const byte zPositiveBit = 4;
static const byte zNegativeBit = 5;

void setPinout() {
  for (int i = 0; i < 6; i++) {
    pinMode(2 + i, OUTPUT);
  }
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(9, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  analogWrite(3, LOW);
  digitalWrite(12, HIGH);
  pinMode(xPositivePin , INPUT_PULLUP);
  pinMode(xNegativePin , INPUT_PULLUP);
  pinMode(yPositivePin , INPUT_PULLUP);
  pinMode(yNegativePin , INPUT_PULLUP);
  pinMode(zPositivePin , INPUT_PULLUP);
  pinMode(zNegativePin , INPUT_PULLUP);
}
void makeMove(int pin, int pin_dir, const int dir, int steps,byte endSwitchBit=0x00) {
  digitalWrite(pin_dir, dir);
  for (int s = 0; s < steps; s++) {
    for (int i = 0; i < 10; i++) {
      if(endSwitchesState & endSwitchBit){
        Serial.println("End switch detect value"+ String(endSwitchesState&endSwitchBit));
      break;
      }
      digitalWrite(pin, HIGH);
      delayMicroseconds(500);
      digitalWrite(pin, LOW);
      delayMicroseconds(500);
      limitSwitches();
    }
  }
}
void control(char ctrl, int steps) {
  switch (ctrl) {
    case 'w':
      makeMove(3, 6, HIGH, steps,yPositiveBit);
      break;
    case 's':
      makeMove(3, 6, LOW, steps,yNegativeBit);
      break;
    case 'a':
      makeMove(2, 5, HIGH, steps,xNegativeBit);
      break;
    case 'd':
      makeMove(2, 5, LOW, steps,xPositiveBit);
      break;
    case 'y':
      makeMove(4, 7, HIGH, steps,zPositiveBit);
      break;
    case 'h':
      makeMove(4, 7, LOW, steps,zNegativeBit);
      break;
    case 'm':
        sizeMeasure();
        break;
    case 'o':
      if (!on) {
        digitalWrite(12, LOW);
        on = !on;
      } else {
        digitalWrite(12, HIGH);
        on = !on;
      }
      break;
    case 'p':
      if (!cool) {
        analogWrite(3, LOW);
        cool = !cool;
      } else {
        analogWrite(3, HIGH);
        cool = !cool;
      }
      break;
  }
}
void limitSwitches() {
  if (digitalRead(xPositivePin) == LOW) {
    delay(20);
    bitWrite(endSwitchesState,xPositiveBit,HIGH);
  } else if (digitalRead(xNegativePin) == LOW) {
    delay(20);
    bitWrite(endSwitchesState,xNegativeBit,HIGH);
  }
  if (digitalRead(yPositivePin) == LOW) {
    delay(20);
    bitWrite(endSwitchesState,yPositiveBit,HIGH);
  } else if (digitalRead(yNegativePin) == LOW) {
    delay(20);
    bitWrite(endSwitchesState,yNegativeBit,HIGH);
  }
  if (digitalRead(zPositivePin) == LOW) {
    delay(20);
    bitWrite(endSwitchesState,zPositiveBit,HIGH);
  } else if (digitalRead(zNegativePin) == LOW) {
    delay(20);
    bitWrite(endSwitchesState,zNegativeBit,HIGH);
  } 
  
  if (digitalRead(xPositivePin) != LOW) {
    delay(20);
    bitWrite(endSwitchesState,xPositiveBit,LOW);
  }
  if (digitalRead(xNegativePin) != LOW) {
    delay(20);
    bitWrite(endSwitchesState,xNegativeBit,LOW);
  }
  if (digitalRead(yPositivePin) != LOW) {
    delay(20);
    bitWrite(endSwitchesState,yPositiveBit,LOW);
  }
  if (digitalRead(yNegativePin) != LOW) {
    delay(20);
    bitWrite(endSwitchesState,yNegativeBit,LOW);
  }
  if (digitalRead(zPositivePin) != LOW) {
    delay(20);
    bitWrite(endSwitchesState,zPositiveBit,LOW);
  }
  if (digitalRead(zNegativePin) != LOW) {
    delay(20);
    bitWrite(endSwitchesState,zNegativeBit,LOW);
  }
}

void sizeMeasure() {
    bool start = false;
    bool end = false;
    int x = 0,y=0;
    do {
        if (!start) {
            if (0 == bitRead(endSwitchesState, xNegativeBit)) {
                makeMove(2, 5, HIGH, 1);
            }
            else
            if (0 == bitRead(endSwitchesState, yNegativeBit)) {
                makeMove(3, 6, LOW, 1);
            }
            else
                start = !start;
            limitSwitches();
        }
        else {
            if (!end) {
                if (0 == bitRead(endSwitchesState, xPositiveBit)) {
                    makeMove(2, 5, LOW, 1);
                    x++;
                }else
                if (0 == bitRead(endSwitchesState, yPositiveBit)) {
                    makeMove(3, 6, HIGH, 1);
                    y++;
                }
                else
                    end = !end;
                limitSwitches();
            }

        }
        

    } while (!end);
    Serial.println("x: " + String(x) + "   y:" + String(y));
}

void setup() {
  Serial.begin(115200);
  setPinout();
}

void loop() {
  if (Serial.available()) {
    s = Serial.readString();
    numOfSteps = s.charAt(1) - '0';
    control(s.charAt(0), numOfSteps);
    Serial.println(s);
    Serial.flush();
    s="";
  }
}
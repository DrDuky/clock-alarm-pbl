#include <RTClib.h>         // Real-time clock library
#include <LiquidCrystal.h>  // LCD display library
#include "Keypad.h"         // Keypad library

RTC_DS1307 rtc;

// LCD pins: RS, E, D4, D5, D6, D7
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 13, 6, 5, 4 };
byte colPins[COLS] = { 3, 2, A0 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Alarm and buzzer variables
const int buzzerPin = A1;
bool alarmTriggered = false;
bool alarmSet = false;
int alarmHour = 0;
int alarmMinute = 0;
unsigned long lastCountdownUpdate = 0;
unsigned long starTime = 0;
bool starOnce = false;

byte cursor_x = 0;
byte cursor_y = 0;

unsigned long previousMillis = 0;
const long interval = 1000;
bool hi = true;


// Buzzer beep
void ringBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(750);
  digitalWrite(buzzerPin, LOW);
  delay(750);
  digitalWrite(buzzerPin, HIGH);
}

// Show current time on LCD
void ShowTime(int hour, int minute, int second) {
  lcd.setCursor(0, 0);
  lcd.print(" TIME: ");
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) lcd.print("0");
  lcd.print(second);
}

// Prompt user to enter MMSS and set countdown
void setAlarm() {
  int hours;
  int minutes;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Enter time for");
  lcd.setCursor(0, 1);
  lcd.print("alarm (HOUR:MIN)");

  delay(1250);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("v       move: *");
  lcd.setCursor(0, 1);
  lcd.print("00:00  enter: #");

  String input = "";
  int numbers[4] = { 0, 0, 0, 0 };

  while (true) {
    char key = keypad.getKey();
    //lcd.setCursor(cursor_x, cursor_y);
    if (key != NO_KEY) {
      if (isDigit(key)) {
        lcd.setCursor(cursor_x, 0);
        lcd.print(" ");

        lcd.setCursor(cursor_x, 1);
        lcd.print(key);
        switch (cursor_x) {
          case 0:
            Serial.println(key);
            numbers[0] = key - '0';
            break;
          case 1:
            Serial.println(key);
            numbers[1] = key - '0';
            break;
          case 3:
            Serial.println(key);
            numbers[2] = key - '0';
            break;
          case 4:
            Serial.println(key);
            numbers[3] = key - '0';
            break;
        }
        cursor_x++;
        movecursor();
      } else if (key == '*') {
        lcd.setCursor(cursor_x, 0);
        lcd.print(" ");

        cursor_x++;
        movecursor();
      } else if (key == '#') {
        for (int i = 0; i < 4; i++) {
          input = input + numbers[i];
        }
        hours = input.substring(0, 2).toInt();
        minutes = input.substring(2, 4).toInt();
        Serial.println("lol");

        Serial.println(hours);
        Serial.println(minutes);
        if (hours > 24 || minutes > 60) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("invalid time");
          delay(1500);
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Enter time for");
          lcd.setCursor(0, 1);
          lcd.print("alarm (HOUR:MIN)");

          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("v       move: *");
          lcd.setCursor(0, 1);
          lcd.print("00:00  enter: #");
          cursor_x = 0;
          input = "";
        } else break;
      }
    }
  }

  alarmHour = hours;
  alarmMinute = minutes;
  alarmSet = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm set for:");
  lcd.setCursor(0, 1);
  if (alarmHour < 10) lcd.print("0");
  lcd.print(alarmHour);
  lcd.print(":");
  if (alarmMinute < 10) lcd.print("0");
  lcd.print(alarmMinute);
  delay(2000);
  lcd.clear();
}
void movecursor() {
  if (cursor_x == 2) {
    cursor_x++;
  } else if (cursor_x >= 5) {
    cursor_x = 0;
  }
  lcd.setCursor(cursor_x, 0);
  lcd.print('v');
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int seconds = now.second();

  ShowTime(hour, minute, seconds);

  if (alarmSet && !alarmTriggered) {
    if (hour == alarmHour && minute == alarmMinute) {
      alarmTriggered = true;
      alarmSet = false;
      while (true) {
        char key = keypad.getKey();
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          if (hi) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("! ALARM ACTIVE !");
            lcd.setCursor(0, 1);
            lcd.print(" Stop alarm: #");
            hi = false;
          } else {
            lcd.clear();
            hi = true;
          }
        }
        ringBuzzer();
        if (key != NO_KEY && key == '#') {
          alarmTriggered = false;
          alarmSet = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(" Alarm  stopped");
          delay(750);
          lcd.clear();

          break;
        }
      }

    } else {
      int currentTotal = hour * 60 + minute;
      int alarmTotal = alarmHour * 60 + alarmMinute;
      int diff = alarmTotal - currentTotal;
      if (diff < 0) diff += 24 * 60;  // if alarm is next day

      int hLeft = diff / 60;
      int mLeft = diff % 60;

      lcd.setCursor(0, 1);
      lcd.print(" Left: ");
      if (hLeft < 10) lcd.print("0");
      lcd.print(hLeft);
      lcd.print(":");
      if (mLeft < 10) lcd.print("0");
      lcd.print(mLeft);
      lcd.print("   ");
    }
  } else if (!alarmTriggered) {
    lcd.setCursor(0, 1);
    lcd.print(" Set alarm: *   ");
  }
  // Keypad input
  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.println(key);
    if (key == '*') {
      setAlarm();
    }
  }

  // Control buzzer
  if (alarmTriggered) {
    ringBuzzer();
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

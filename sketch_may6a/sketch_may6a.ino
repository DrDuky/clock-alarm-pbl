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
bool countdownActive = false;
unsigned long alarmStartTime = 0;
unsigned long alarmDuration = 0;
unsigned long lastCountdownUpdate = 0;

// Buzzer beep
void ringBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(200);
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Enter MMSS:");

  String input = "";
  while (input.length() < 4) {
    char key = keypad.getKey();
    if (key != NO_KEY && isDigit(key)) {
      input += key;
      lcd.setCursor(input.length() - 1, 1);
      lcd.print(key);
    }
  }

  int minutes = input.substring(0, 2).toInt();
  int seconds = input.substring(2, 4).toInt();
  alarmDuration = (minutes * 60UL + seconds) * 1000UL;
  alarmStartTime = millis();
  countdownActive = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm in ");
  lcd.print(minutes);
  lcd.print("m ");
  lcd.print(seconds);
  lcd.print("s");
  delay(1500);
  lcd.clear();
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int seconds = now.second();

  ShowTime(hour, minute, seconds);

  // Countdown logic
  if (countdownActive) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - alarmStartTime;

    if (elapsedTime >= alarmDuration) {
      alarmTriggered = true;
      countdownActive = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!! ALARM ACTIVE !!");
      lcd.setCursor(0, 1);
      lcd.print(" Stop alarm: #  ");
    } else if (currentTime - lastCountdownUpdate >= 1000) {
      lastCountdownUpdate = currentTime;

      unsigned long remainingTime = alarmDuration - elapsedTime;
      int totalSeconds = remainingTime / 1000;
      int remMin = totalSeconds / 60;
      int remSec = totalSeconds % 60;

      lcd.setCursor(0, 1);
      lcd.print(" Left: ");
      if (remMin < 10) lcd.print("0");
      lcd.print(remMin);
      lcd.print(":");
      if (remSec < 10) lcd.print("0");
      lcd.print(remSec);
      lcd.print("  ");
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

    if (alarmTriggered && key == '#') {
      alarmTriggered = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Alarm  stopped");
      delay(1000);
      lcd.clear();
    }
  }

  // Control buzzer
  if (alarmTriggered) {
    ringBuzzer();
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

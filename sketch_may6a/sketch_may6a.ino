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

int hours;
int minutes;

byte cursor_x = 0;
byte cursor_y = 0;



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
          lcd.setCursor(3, 0);
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
        } else break;
      }
    }
  }

  DateTime now = rtc.now();
  int seconds = now.second();
  int minute = now.minute();
  int hour = now.hour();

  long alarmSeconds = ((long)hours * 3600) + ((long)minutes * 60);
  Serial.println(alarmSeconds);
  long currentSeconds = ((long)hour * 3600) + ((long)minute * 60) + (long)(seconds);
  Serial.println(currentSeconds);
  long delaySeconds = alarmSeconds - currentSeconds;
  Serial.println(delaySeconds);
  Serial.println(delaySeconds);

  if (delaySeconds <= 0) {
    Serial.println(delaySeconds);
    delaySeconds += 24 * 3600;  // Alarm is for the next day
    Serial.println(delaySeconds);
  }

  alarmDuration = delaySeconds;
  alarmStartTime = millis();
  countdownActive = true;


  /*
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("select weekday");
  lcd.setCursor(1, 1);
  lcd.print("M<T W T F S S");
  while(true){
    char key = keypad.getKey();
    if (key != NO_KEY && isDigit(key)) {
      if () {
        
      }
    }
  }
  */
  delay(10000);
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

      lcd.setCursor(1, 1);
      lcd.print("Left: ");
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

#include <RTClib.h>
#include <LiquidCrystal.h>
#include "Keypad.h"

RTC_DS1307 rtc;
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4;  // number of rows
const byte COLS = 3;  // number of columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 13, 6, 5, 4 };  // row pinouts of the keypad
byte colPins[COLS] = { 3, 2, A0 };     // column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buzzerPin = A1;     // Buzzer pin
bool alarmTriggered = false;  // Keeps track if alarm is active

void ringBuzzer() {  // defining function called ringBuzzer
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(200);
  digitalWrite(buzzerPin, HIGH);
}

void ShowTime(int hour, int minute, int second) {
  lcd.setCursor(0, 0);
  lcd.print(" TIME: ");
  if (hour < 10) lcd.print(" 0");
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) lcd.print("0");
  lcd.print(second);
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);    // Set buzzer as output
  digitalWrite(buzzerPin, LOW);  // Start with buzzer OFF

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set RTC time from computer
}

void loop() {
  static bool tested = false;  // This keeps track if we already set the alarm once  (REMOVE THIS LATER, THIS IS JUST TO TEST BUZZER IN THE BEGINNING)
  if (!tested) {
    lcd.setCursor(0, 1);
    lcd.print(" Stop alarm: #");
    alarmTriggered = true;
    tested = true;  // So it doesn’t repeat again
  }
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int seconds = now.second();

  ShowTime(now.hour(), now.minute(), now.second());

  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.println(key);

    // If alarm is triggered and user presses '#', stop the buzzer
    if (alarmTriggered && key == '#') {
      alarmTriggered = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Alarm  stopped");
      delay(1000);  // show for 1 second
      lcd.clear();
    }
  }
  // Control the buzzer based on alarm state
  if (alarmTriggered) {
    ringBuzzer();
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

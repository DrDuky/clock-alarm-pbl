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
  { '#', '0', '*' }
};

byte rowPins[ROWS] = { 13, 6, 5, 4 };  // row pinouts of the keypad R1 = D8, R2 = D7, R3 = D6, R4 = D5
byte colPins[COLS] = { 3, 2, A0 };      // column pinouts of the keypad C1 = D4, C2 = D3, C3 = D2
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  // put your setup code here, to run once:
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();

  lcd.setCursor(0, 0);
  lcd.print("TIME : ");
  lcd.print(hour);
  lcd.print(":");
  lcd.print(now.minute());


  {
    char key = keypad.getKey();
    if (key != NO_KEY)
      Serial.println(key);
  }
}

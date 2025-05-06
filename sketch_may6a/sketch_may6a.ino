#include <RTClib.h>
#include <LiquidCrystal.h>

RTC_DS1307 rtc;
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
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
  lcd.print(minute);
}

#include <RTClib.h>         // real-time clock (RTC)
#include <LiquidCrystal.h>  // LCD screen
#include "Keypad.h"         // keypad

RTC_DS1307 rtc;  // Creates the RTC object

// LCD pin setup (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  // Initialize LCD

// Keypad layout and wiring
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 13, 6, 5, 4 };                                    // row pins of keypad
byte colPins[COLS] = { 3, 2, A0 };                                       // column pins of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  // Initialize keypad

const int buzzerPin = A1;  // Pin connected to buzzer

// Alarm-related variables
bool alarmTriggered = false;
bool alarmSet = false;
int alarmHour = 0;
int alarmMinute = 0;

byte cursor_x = 0;                 // Cursor (V) position for alarm input on LCD
unsigned long previousMillis = 0;  // Timer for blinking effect
const long interval = 1000;
bool blinkOn = true;  // Used for blinking the LCD text (! ALARM ACTIVE !)

// Defining Buzzer function called ringBuzzer (beep pattern)
void ringBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(750);
  digitalWrite(buzzerPin, LOW);
  delay(750);
  digitalWrite(buzzerPin, HIGH);
}

// Defining the function to show current time on LCD
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

//  the alarm time using keypad
void setAlarm() {
  // Setup variables + initial message
  int hours, minutes;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Enter time for");
  lcd.setCursor(0, 1);
  lcd.print("alarm (HOUR:MIN)");
  delay(1250);
  // Shows the time input interface
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("v       move: *");  // tells the user how to move the cursor by pressing *
  lcd.setCursor(0, 1);
  lcd.print("00:00  enter: #");  // shows the time format for the input and how to confirm input
  // Stores the input
  String input = "";
  int numbers[4] = { 0, 0, 0, 0 };  // Store HOUR:MIN digits (array)

  while (true) {  // forever loop that keeps cheking the keypad as long as the condition is true
    char key = keypad.getKey();
    // if a key is pressed
    if (key != NO_KEY) {  // runs this only if a key was pressed (key is NOT NO_KEY)
                          // if the key pressed is a number from 0-9
      if (isDigit(key)) {
        lcd.setCursor(cursor_x, 0);
        lcd.print(" ");  // Removes the arrow v
        lcd.setCursor(cursor_x, 1);
        lcd.print(key);      // Show digit
        switch (cursor_x) {  // saves the digit into the array
          case 0: numbers[0] = key - '0'; break;
          case 1: numbers[1] = key - '0'; break;
          case 3: numbers[2] = key - '0'; break;
          case 4: numbers[3] = key - '0'; break;
        }
        cursor_x++;
        movecursor();  // Moves cursor manually
      } else if (key == '*') {
        lcd.setCursor(cursor_x, 0);
        lcd.print(" ");
        cursor_x++;
        movecursor();
      } else if (key == '#') {  // confirms the time
        for (int i = 0; i < 4; i++) input += numbers[i];
        hours = input.substring(0, 2).toInt();
        minutes = input.substring(2, 4).toInt();

        // Checks if the time is valid
        if (hours > 24 || minutes > 60) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("invalid time");
          delay(1500);

          // Resets the screen if time is invalid
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
        } else break;  // If time is valid, breaks the while (true) loop
      }
    }
  }

  // Saves alarm time in global variables and displays the alarm set
  alarmHour = hours;
  alarmMinute = minutes;
  alarmSet = true;
  // Shows confirmation on LCD
  lcd.clear();
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

// Move input cursor and skip colon (:)
void movecursor() {
  if (cursor_x == 2) cursor_x++;         // Skip colon
  else if (cursor_x >= 5) cursor_x = 0;  // Loops back to position 0
  lcd.setCursor(cursor_x, 0);
  lcd.print('v');  // Prints the arrow symbol on top
}

void setup() {
  lcd.begin(16, 2);  // LCD Setup
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);    // Sets Buzzer pin an output
  digitalWrite(buzzerPin, LOW);  // Makes sure buzzer is off

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;  // Stops the program if RTC is not found
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Sets the current time (once during upload)
}
void loop() {
  DateTime now = rtc.now();  // Gets current time
  int hour = now.hour();
  int minute = now.minute();
  int seconds = now.second();

  ShowTime(hour, minute, seconds);  // Always shows the time

  // If alarm is set and did not get triggered yet
  if (alarmSet && !alarmTriggered) {
    // Checks if current time matches alarm time
    if (hour == alarmHour && minute == alarmMinute) {
      alarmTriggered = true;
      alarmSet = false;

      // Alarm loop (Shows blinking message and buzzer)
      while (true) {
        char key = keypad.getKey();
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {  // blinking message
          previousMillis = currentMillis;
          lcd.clear();
          if (blinkOn) {
            lcd.setCursor(0, 0);
            lcd.print("! ALARM ACTIVE !");
            lcd.setCursor(0, 1);
            lcd.print(" Stop alarm: #");
            blinkOn = false;
          } else {
            blinkOn = true;
          }
        }

        ringBuzzer();  // Sounds the buzzer

        // Stop alarm when # is pressed
        if (key == '#') {
          alarmTriggered = false;
          lcd.clear();
          lcd.print(" Alarm  stopped");
          delay(750);
          lcd.clear();
          break;
        }
      }

    } else {
      // Countdown display to alarm
      int currentTotal = hour * 60 + minute;
      int alarmTotal = alarmHour * 60 + alarmMinute;
      int diff = alarmTotal - currentTotal;
      if (diff < 0) diff += 24 * 60;  // Next-day alarm

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

  } else if (!alarmTriggered) {  // Shows default message if no alarm is set
    lcd.setCursor(0, 1);
    lcd.print(" Set alarm: *   ");
  }

  // If * is pressed, runs setAlarm()
  char key = keypad.getKey();
  if (key == '*') {
    setAlarm();
  }

  // Buzzer stays off if alarm not triggered
  if (!alarmTriggered) {
    digitalWrite(buzzerPin, LOW);
  }
}

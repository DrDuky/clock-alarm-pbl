#include <RTClib.h>         // Library to use the real-time clock (RTC)
#include <LiquidCrystal.h>  // Library to control the LCD display
#include "Keypad.h"         // Library to use the keypad

RTC_DS1307 rtc;  // Create an RTC object

// LCD pin connections (RS, E, D4, D5, D6, D7)
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define the keypad layout (4 rows, 3 columns)
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// Pin connections for keypad rows and columns
byte rowPins[ROWS] = { 13, 6, 5, 4 };
byte colPins[COLS] = { 3, 2, A0 };

// Create keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buzzerPin = A1;     // Buzzer connected to analog pin A1
bool alarmTriggered = false;  // Keeps track if alarm is on

// Function to make the buzzer sound
void ringBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(200);
  digitalWrite(buzzerPin, HIGH);
}

// Function to display the current time on the LCD
void ShowTime(int hour, int minute, int second) {
  lcd.setCursor(0, 0);
  lcd.print(" TIME: ");
  if (hour < 10) lcd.print(" 0");  // Add leading zero if needed
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) lcd.print("0");
  lcd.print(second);
}

void setup() {
  lcd.begin(16, 2);              // Initialize LCD with 16 columns and 2 rows
  Serial.begin(9600);            // Start serial communication for debugging
  pinMode(buzzerPin, OUTPUT);    // Set buzzer pin as output
  digitalWrite(buzzerPin, LOW);  // Start with buzzer off

  if (!rtc.begin()) {  // Check if RTC is connected
    Serial.println("Couldn't find RTC");
    while (1)
      ;  // Stay stuck here if RTC is not found
  }

  // Sync RTC with the computer time (only runs once when uploaded)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  static bool tested = false;

  // TEMPORARY: auto-trigger the alarm once on startup for testing
  if (!tested) {
    lcd.setCursor(0, 1);
    lcd.print(" Stop alarm: #");  // Show how to stop the alarm
    alarmTriggered = true;        // Trigger the alarm
    tested = true;                // Prevent re-triggering
  }

  // Get current time from RTC
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();
  int seconds = now.second();

  // Show current time on LCD
  ShowTime(hour, minute, seconds);

  // Check if any key is pressed
  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.println(key);  // Print key to Serial Monitor

    // If alarm is ringing and user presses '#', turn it off
    if (alarmTriggered && key == '#') {
      alarmTriggered = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Alarm  stopped");
      delay(1000);  // Show message for 1 second
      lcd.clear();  // Clear the display
    }
  }

  // Control the buzzer depending on alarm state
  if (alarmTriggered) {
    ringBuzzer();
  } else {
    digitalWrite(buzzerPin, LOW);  // Make sure buzzer is off
  }
}

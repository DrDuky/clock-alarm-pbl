# clock-alarm-pbl

**⏰ Smart Real-Time Alarm Clock**

A simple Arduino-based IoT project that lets users set an alarm using a keypad. When the alarm time is reached, a buzzer sounds and a blinking message appears on the LCD. Built with minimal components and full real-time functionality.

![image](https://github.com/user-attachments/assets/a086b586-abb7-44c7-902a-5d3408190861)


## 📌 Features

| ✓ | Description |
|----|-------------|
| **Real-Time Tracking** | DS1307 RTC module keeps accurate time. |
| **Keypad Alarm Input** | User sets alarm using 3x4 keypad. |
| **Live LCD Display** | Continuously displays current time and alarm countdown. |
| **Alarm Notification** | Triggers buzzer + blinking message when time is reached. |
| **Manual Dismiss** | Press `#` to stop the alarm. |
| **Auto Reset** | System resets automatically once alarm is stopped. |
(GNU-licensed code)
## 🧰 Components Used

- Arduino Uno  
- 3x4 Matrix Keypad  
- RTC Module (DS1307)  
- 16x2 LCD Display  
- Active Buzzer (A1)  
- 330Ω resistor for LCD backlight  
- Breadboard + jumper wires  

## 🔌 Pin Connections

![image](https://github.com/user-attachments/assets/30eed883-c717-48bf-bd6e-45604b970db4)



| **Symbol** | **Component**         | **Arduino Pin(s)**                                                                                                                                     | **Notes**                                                                 |
|------------|------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------|
| `KPD`      | 3x4 Keypad (input)     | Rows → 13, 6, 5, 4<br>Cols → 3, 2, A0                                                                                                                   | Reads 4-digit alarm input. Handled by Keypad library.                    |
| `RTC`      | RTC Module (DS1307)    | SDA → A4, SCL → A5<br>VCC → 5V, GND → GND                                                                                                               | Uses I2C (no pinMode). Powered by 5V. Handled by RTClib library.         |
| `BZ1`      | Active Buzzer          | Positive → A1<br>Negative → GND                                                                                                                         | Digital output buzzer. Simple HIGH/LOW control.                          |
| `LCD`      | 16x2 LCD Display       | RS → 7, EN → 8, D4–D7 → 9–12<br>VSS & RW → GND<br>VDD → 5V<br>V0 → GND (1kΩ)<br>Backlight: A → 5V (330Ω), K → GND                                       | Uses LiquidCrystal library for 4-bit control.  

## 🧠 How It Works

1. LCD shows the real-time clock continuously using the RTC.
2. User presses `*` to enter alarm setup.
3. User enters alarm time via keypad (HH:MM) and confirms with `#`.
4. The system checks the time in every loop. When matched, buzzer sounds and screen blinks.
5. Pressing `#` stops the alarm and resets the system.

![image](https://github.com/user-attachments/assets/1c839ec6-0c2b-4f93-9299-09f568e98dcb)


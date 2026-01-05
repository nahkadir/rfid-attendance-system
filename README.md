# RFID Attendance System
## Overview
The RFID Attendance System automates attendance management in educational institutions using RFID cards and an ESP32 microcontroller. 
It replaces manual, paper-based attendance with a fast, accurate and secure cloud-based solution. 
Attendance is recorded in real time and uploaded to Google Sheets, reducing errors, saving time, and preventing proxy attendance.

## Project Objectives
- Eliminate manual attendance marking
- Ensure accurate and secure attendance records
- Store and monitor attendance on the cloud

## Target Audience
- Students – automatic attendance recording
- Teachers – no manual attendance work
- Educational Institutions – centralized cloud records

## Features
1. RFID-Based Identification
Each student is assigned a unique RFID card (UID) used for identification.
2. Student Data Mapping
Student details (name, roll number, class/section) are mapped to RFID UIDs.
3. Attendance Verification
Displays student name and status on LCD
Records date & time automatically
Uploads attendance to Google Sheets
4. Duplicate Scan Detection
Prevents multiple attendance entries for the same card.
5. Audio & Visual Feedback
Green LED + short beep → Valid scan
Red LED + double beep → Duplicate scan
6. Cloud Integration
Attendance is uploaded via Wi-Fi to Google Sheets using Google Apps Script.

## Hardware Components
ESP32 Microcontroller

RFID Reader (RC522)

RFID Cards

OLED / I2C LCD Display

LEDs (Red & Green)

Buzzer

Breadboard & Jumper Wires

USB Power Cable

## Software & Tools

Visual Studio Code

PlatformIO

Google Apps Script

Google Sheets

## Libraries Used

MFRC522

WiFi

HTTPClient

LiquidCrystal_I2C

time.h

## Connections
# RFID RC522 Connections

| Component | ESP32 Pin |
| --------- | --------- |
| SDA       | GPIO 22   |
| SCK       | GPIO 18   |
| MOSI      | GPIO 23   |
| MISO      | GPIO 19   |
| RST       | GPIO 21   |
| GND       | GND       |
| 3.3V      | 3.3V      |

# LEDs Connections
| Component     | ESP32 Pin |
| ------------- | --------- |
| Green LED (+) | GPIO 26   |
| Green LED (-) | GND       |
| Red LED (+)   | GPIO 25   |
| Red LED (-)   | GND       |

# Buzzer Connection
| Component  | ESP32 Pin |
| ---------- | --------- |
| Buzzer (+) | GPIO 27   |
| Buzzer (-) | GND       |

# LCD (I2C) Connections
| Component | ESP32 Pin |
| --------- | --------- |
| SDA       | GPIO 33   |
| SCL       | GPIO 32   |
| VCC       | 3.3V      |
| GND       | GND       |


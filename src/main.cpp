#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
// LCD object created at I2c address 0x27 (16 col and 2 rows)

char ssid[] = "iPhone";
char pass[] = "pass1234";

// MFRC522 object created for RFID communication
#define SS_PIN 22
#define RST_PIN 21
MFRC522 rfid(SS_PIN, RST_PIN);

#define BUZZER_PIN 27
#define GREEN_LED 26   
#define RED_LED   25   

struct Student {
  byte uid[10];
  byte uidSize;
  const char* roll;
  const char* name;
  const char* classSection;
};

// Add roll number and class section for each student
Student students[] = {
  {{0xA3, 0xA4, 0xF7, 0x0D}, 4, "1097", "Zainab Sultan", "CS-B"},
  {{0xB3, 0x8E, 0xD3, 0x0F}, 4, "1085", "Qura Tul Ain", "CS-B"}
};
int studentCount = sizeof(students) / sizeof(students[0]);

String lastUID = "";
// last scanned UID stored to prevent duplicates

// to reset RFID reader in case of errors or failed reads
void reviveRC522() {
  digitalWrite(RST_PIN, LOW);
  delay(50);
  digitalWrite(RST_PIN, HIGH);
  delay(50);
}

Student* getStudent(byte* uid, byte uidSize) {
  for (int i = 0; i < studentCount; i++) {
    if (students[i].uidSize != uidSize) continue;
    bool match = true;
    for (byte j = 0; j < uidSize; j++) {
      if (students[i].uid[j] != uid[j]) {
        match = false;
        break;
      }
    }
    if (match) return &students[i];
  }
  return nullptr;
}

void buzzerShort() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzerDouble() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
  delay(150);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
}

#define GOOGLE_SCRIPT_URL "https://script.google.com/macros/s/AKfycbxzG1-SlJm_Bed0PKVVa2jtNYOHqllJGljfJryto33CVKv3FsP1Bkv-thEZvihmdOf1/exec"

// sends HTTP GET request to Google Script to log attendance
void sendToGoogleSheet(String roll, String name, String classSection, String date, String time) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Only encode spaces in the name
    name.replace(" ", "%20");

    String url = String(GOOGLE_SCRIPT_URL) +
                 "?roll=" + roll +
                 "&name=" + name +
                 "&classSection=" + classSection +
                 "&date=" + date +
                 "&time=" + time;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Google Sheet updated");
    } else {
      Serial.println("Failed to update Google Sheet");
    }

    http.end();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Initialize I2C (SDA=33, SCL=32)
  Wire.begin(33, 32);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Attendance");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");

  // WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected! IP: " + WiFi.localIP().toString());

  // Time
  configTime(5 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // RFID
  SPI.begin(18, 19, 23);
  pinMode(RST_PIN, OUTPUT);
  reviveRC522();
  rfid.PCD_Init();

  Serial.println("RFID Ready...");
  
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan Your Card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println("Read failed, reviving...");
    reviveRC522();
    rfid.PCD_Init();
    return;
  }

  Serial.print("Scan successful");

  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uidStr += " ";
  }
  Serial.println();

  if (uidStr == lastUID) {
    Serial.println("Duplicate scan detected!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DUPLICATE!");
    lcd.setCursor(0, 1);
    lcd.print("Already Scanned");

    digitalWrite(RED_LED, HIGH);
    buzzerDouble();
    delay(2000);
    digitalWrite(RED_LED, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Your Card");

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  Student* student = getStudent(rfid.uid.uidByte, rfid.uid.size);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  char dateStr[15];
  char timeStr[15];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  String record = (student ? student->name : "Unknown");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("VERIFIED!");
  lcd.setCursor(0, 1);
  if (student) {
    lcd.print(student->name);
  } else {
    lcd.print("Unknown Card");
  }

  digitalWrite(GREEN_LED, HIGH);
  buzzerShort();
  delay(2000);
  digitalWrite(GREEN_LED, LOW);

  // Send to Google Sheet
  sendToGoogleSheet(
    student ? student->roll : "Unknown",
    student ? student->name : "Unknown",
    student ? student->classSection : "Unknown",
    String(dateStr),
    String(timeStr)
  );

  Serial.println("Sent to Google Sheet: " + record);

  lastUID = uidStr;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan Your Card");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
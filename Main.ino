#include <SPI.h>
#include <SD.h>
#include <math.h>
 
#include "DHT.h"
#include "RTClib.h"
 
#define DHTPIN 2
#define DHTTYPE DHT22
#define BUTTON_PIN 8
#define LED_PIN 9
//#define TRANSISTOR_PIN 3
#define SD_CS 10
#define ANALOG_PIN A0
 
String fileName = "";
String lastFileName;
 
// Constants
const float R_REF = 10000.0;      // 10k reference resistor
const float Vcc = 5.0;
const unsigned long LOG_INTERVAL = 120000UL; // 2 minutes
const float surelyDry = 4.95; // si voltage plus bas que surelyDry, probablement sec (verifie a chaque fois que le systeme commence a verifier), ajouter plus tard
 
// Dry detection
const float DRY_THRESHOLD = 0.02;  // volt
const int STABLE_READS_REQUIRED = 3;
 
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;
 
bool loggingActive = false;
bool isDry = false;
 
unsigned long lastLogTime = 0;
unsigned long initialTime = 0;
 
float resistance;
float lastResistance = -1;
int stableCount = 0;
 
float hum;
float temp;
 
unsigned long buttonPressStart = 0;
bool lastButton = LOW;
bool longPress = false;
 
void setup() {
  Serial.begin(9600);
 
  dht.begin();
  pinMode(BUTTON_PIN, INPUT);   // using external pull-down
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
 
  //pinMode(TRANSISTOR_PIN, OUTPUT);
  //digitalWrite(TRANSISTOR_PIN, LOW); // sensors OFF initially
 
  if (!SD.begin(SD_CS)) {
    Serial.println("SD initialization failed!");
  } else {
    Serial.println("SD OK");
  }
 
  if (!rtc.begin()) {
    Serial.println("RTC not found!");
  }
 
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
  // Create data file if new
 
}
 
void loop() {
 
  // BUTTON HANDLING (simple toggle)
  bool button = digitalRead(BUTTON_PIN);
 
  // Detect rising edge (press)
  if (button == HIGH && lastButton == LOW) {buttonPressStart = millis(); longPress = false;}
 
  delay(50);
 
  long buttonDiff = millis() - buttonPressStart;
 
  if (button == HIGH && buttonDiff >= 3000 && longPress == false) {printSdData(); longPress = true; delay(500);} else if (button == LOW && lastButton == HIGH && longPress == false) {
  //if (button == HIGH && lastButton == LOW) {
 
    if (!loggingActive) {
    initialTime = millis();
    isDry = false;
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    }
 
    if (loggingActive) {
      delay(500);
      digitalWrite(LED_PIN, HIGH);
      delay(3000);
      digitalWrite(LED_PIN, LOW);
    }
 
    loggingActive = !loggingActive;
    Serial.println(loggingActive ? "Logging STARTED" : "Logging PAUSED");
  }
 
  if (!loggingActive) {
      //digitalWrite(TRANSISTOR_PIN, LOW); // stop powering sensors
  }
 
  lastButton = button;
 
  // DRY STATE
  if (isDry) {
    digitalWrite(LED_PIN, HIGH);
    loggingActive = false;
    //digitalWrite(TRANSISTOR_PIN, LOW);
    //Serial.print("isDry"); // Yanick
    //return;
  }
 
  // MAIN LOGGING LOOP
  if (loggingActive) {
    unsigned long now = millis();
    unsigned long timeDiff = now - initialTime;
    if (now - lastLogTime >= LOG_INTERVAL) {
      lastLogTime = now;
 
      Serial.println("logging"); // Yanick
 
      //digitalWrite(TRANSISTOR_PIN, HIGH); // power sensors
      delay(4000); // allow stabilization ----- changed from 200 to 4000
 
      temp = dht.readTemperature();
      hum  = dht.readHumidity();
      resistance = readResistance();
 
      //Serial.println(hum); Serial.print(temp); Serial.print(resistance); // Yanick
 
      logToSD(timeDiff, temp, hum, resistance);
 
      checkDry(resistance); // detect whether object is stable/dry
 
      //digitalWrite(TRANSISTOR_PIN, LOW); // power off sensors
    }
  }
}
 
////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////
 
float readResistance() {
  int adc = analogRead(ANALOG_PIN);
  float vout = (adc / 1023.0) * 5.0;
  /* if (vout > 4.9) return 999999; // open circuit
  float R = R_REF * (vout / (5.0 - vout)); */
  return vout; // changed from R to vout
}
 
void logToSD(unsigned long timeDiff, float temp, float hum, float resistance) {
  DateTime now = rtc.now();
 
  if (fileName == "") {fileName = createFileName();}
  Serial.print(fileName);
 
  lastFileName = fileName;
 
  if (!SD.exists(fileName)) {File logFile = SD.open(fileName, FILE_WRITE); if (logFile) {logFile.println("Time,TemperatureC,Humidity,Voltage"); logFile.close();}}
 
  File logFile = SD.open(fileName, FILE_WRITE);
  if (logFile) {
    logFile.print(floor(timeDiff/1)); logFile.print(",");
    logFile.print(temp); logFile.print(",");
    logFile.print(hum);  logFile.print(",");
    logFile.println(resistance);
    logFile.close();
  } else {Serial.print("logging error");}
 
  Serial.print("Logged: ");
  Serial.print(temp); Serial.print(" C, ");
  Serial.print(hum);  Serial.print(" %, ");
  Serial.print(resistance); Serial.println(" volt");
}
 
void checkDry(float resistance) {
  if (lastResistance < 0) {lastResistance = resistance; return;}
 
  float diff = (resistance - lastResistance);
 
  if (diff < DRY_THRESHOLD) stableCount++;
  else stableCount = 0;
 
  lastResistance = resistance;
 
  if (stableCount >= STABLE_READS_REQUIRED) {
    isDry = true;
    fileName = "";
    stableCount = 0;
    Serial.println("Object is dry");
  }
}
 
String twoDigit(int v) {
  if (v < 10) {return "0" + String(v);} else {return String(v);}
}
 
String createFileName() {
 
  DateTime now = rtc.now();
 
  String base = twoDigit(now.year() % 100) + twoDigit(now.month()) + twoDigit(now.day());
 
  for (int i = 1; 1 <= 99; i++) {
    String possible = base + twoDigit(i) + ".CSV";
    if (!SD.exists(possible)) {return possible;}
  }
}
 
 
void printSdData() {
  File root = SD.open("/");
 
  while (true) {
    File entry = root.openNextFile();
      if (!entry) {break;}
      Serial.print("\n");
      Serial.println(entry.name());
      while (entry.available()) {
      Serial.write(entry.read());
    }
    entry.close();
  }
  root.close();
}

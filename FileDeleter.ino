#include <SD.h>
#include <SPI.h>

#define SD_CS 10



void setup() {
  Serial.begin(9600);

 if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed");
    while (1);
  }
  
  File root = SD.open("/");
  if (!root) {Serial.print("root issue");} else {Serial.print("root good");}

  while (true) {
    File entry = root.openNextFile();
      if (!entry) {break;}
      Serial.print(entry.name());
      SD.remove(entry.name());
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}

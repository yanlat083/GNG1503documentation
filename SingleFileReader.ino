#include <SD.h>

#define FILE_NAME "2511211.csv"
#define SPI_CS 10

File myFile;


void setup() {
  Serial.begin(9600);
  if (!SD.begin()) {
    Serial.print("SD not responding"); return;
  } else {Serial.print("SD successful");}

  myFile = SD.open(FILE_NAME, FILE_READ);

  while (myFile.available()) {
    Serial.write(myFile.read());
  }
  myFile.close();

}

void loop() {
  // put your main code here, to run repeatedly:

}

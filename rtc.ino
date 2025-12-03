#include <RTClib.h>

RTC_DS1307 rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

 if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
// le RTC doit avoir une data de reference, il ajuste cette date jusqu'a ce que le Arduino soit off
// lorsque le Arduino est off, la date arrete de changer, donc perd sa precision

}

void loop() {
  DateTime now = rtc.now();
  // put your main code here, to run repeatedly:
     Serial.print("\n");
    Serial.print(now.year(), DEC);
    Serial.print('-');
    Serial.print(now.month(), DEC);
    Serial.print('-');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    delay(1000);
}

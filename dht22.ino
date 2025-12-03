#include "DHT.h"

#define DHTPIN 4 // DHT22 sur pin D2
#define DHTTYPE DHT22   
// Connection: Pin 1 voltage, Pin 2 D2, Pin 3 rien, Pin 4 Ground, Resistor 10 kOhm en parallele avec pin 1 et pin 2

DHT dht(DHTPIN, DHTTYPE); // initialiser le capteur

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000); // Doit attendre au moins 0.25 secondes entre mesures
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();


  // Verifier si les mesures sont presentes
  if (isnan(hum) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


  Serial.println(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
}

#define BUTTON_PIN 8

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);  // using external pull-down
}

void loop() {
  int state = digitalRead(BUTTON_PIN);
  Serial.println(state ? "PRESSED" : "RELEASED");
  delay(200);
}

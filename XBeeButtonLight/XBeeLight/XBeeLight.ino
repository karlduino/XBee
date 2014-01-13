/**
 * XBee button/light combination, as a test
 *
 * here, LED connected to Arduino digital pin 13
 * LED turned on if P received from other XBee
 *
 * Based on code from Robert Faludi, Building wireless sensor networks, Chapter 3
 **/

const int LED = 13;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
}


void loop() {
  if(Serial.available() > 0) {
    char input = Serial.read();

    if(input == 'P') {
      digitalWrite(LED, HIGH);
      delay(10);
    }
  }
  digitalWrite(LED, LOW);
}

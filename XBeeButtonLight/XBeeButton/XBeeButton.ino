/**
 * XBee button/light combination, as a test
 *
 * here, button connected to Arduino digital pin 2 and to ground
 * (using internal pull-up resistor)
 * when pressed, send a P to the other XBee
 *
 * Based on code from Robert Faludi, Building wireless sensor networks, Chapter 3
 **/

const int BUTTON = 2;

void setup() {
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH); // set pull-up resistor
  Serial.begin(9600);
}


void loop() {
  if(!digitalRead(BUTTON)) {
    Serial.print('P');
    delay(10);
  }
}

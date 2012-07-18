/**
 * XBee button/light combination, as a test
 *
 * here, LEDs connected to Arduino digital pins 5, 6, 9, each with 680 Ohm resistor
 * reads message from other XBee and switches on the appropriate light
 *
 * Based on code from Robert Faludi, Building wireless sensor networks, Chapter 3
 **/

const int GREEN = 5;
const int RED = 6;
const int YELLOW = 9;
char input;

void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  digitalWrite(GREEN, LOW);
  digitalWrite(RED, HIGH);
  digitalWrite(YELLOW, LOW);

  Serial.begin(9600);
}


void loop() {
  if(Serial.available() > 0) {
    input = Serial.read();

    if(input == 'G') {
      digitalWrite(GREEN, HIGH);
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, LOW);
    }
    else if(input == 'Y') {
      digitalWrite(GREEN, LOW);
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
    }
    else if(input == 'R') {
      digitalWrite(GREEN, LOW);
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
    }
  }
}

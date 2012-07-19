/**
 * Following the romantic lighting example in
 *   Robert Faludi, Building wireless sensor networks, chapter 4
 *
 * XBee Router/AT with photoresistor attached to AD0
 *
 * XBee Coordinator/API with Arduino & XBee shield, and
 *     green LED   : pin 7
 *     yellow LED  : pin 9
 *     red LED     : pin 11
 **/

const int LED[3] = {7, 9, 11};
const int debugLED = 13;
int analogValue = 0;

void setup() {
  pinMode(debugLED, OUTPUT);
  for(int i=0; i<3; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
  }
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() >= 21) {
    // look for start byte
    if(Serial.read() == 0x7E) {
      digitalWrite(debugLED, HIGH);
      delay(10);
      digitalWrite(debugLED, LOW);

      for(int i=0; i<18; i++) {
        byte discard = Serial.read();
      }
      int analogHigh = Serial.read();
      int analogLow = Serial.read();
      analogValue = analogLow + (analogHigh * 256);
    }
  }

  int which_led_on;

  if(analogValue > 0 && analogValue <= 350) {
    which_led_on = 0;  // green led: make brighter
  }
  else if(analogValue <= 750) { // just right: LED[1] on
    which_led_on = 1;  // yellow led: just right
  }
  else { // too bright
    which_led_on = 2;  // red led: too bright
  }

  for(int i=0; i<3; i++) {
    digitalWrite(LED[i], i==which_led_on);
  }
}

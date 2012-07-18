/**
 * XBee button/light combination, as a test
 *
 * here, button connected to Arduino digital pin 2 with 10k Ohm pull-down resistor
 * with each press, writes message that rotates among R, Y, G
 *
 * Based on code from Robert Faludi, Building wireless sensor networks, Chapter 3
 **/

const int BUTTON = 2;
int button_val = 0;
int prev_button_val = 0;
int state = 0;

void setup() {
  pinMode(BUTTON, INPUT);
  Serial.begin(9600);
}


void loop() {
  button_val = digitalRead(BUTTON);

  if((button_val == HIGH) && (prev_button_val == LOW)) {
      state++;
      if(state > 2) state = 0;

      if(state == 0) {
        Serial.print('R');
      }
      else if (state == 1) {
        Serial.print('Y');
      }
      else if (state == 2) {
        Serial.print('G');
      }

      delay(20);
  }

  prev_button_val = button_val;
}

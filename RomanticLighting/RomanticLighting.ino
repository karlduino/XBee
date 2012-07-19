/**
 * Following the romantic lighting example in
 *   Robert Faludi, Building wireless sensor networks, chapter 4
 *
 * XBee Router/AT with photoresistor attached to AD0
 *    DO set to analog input with ATD02
 *    D1 set to low digital output with ATD14
 *
 * XBee Coordinator/API with Arduino & XBee shield, and
 *     green LED   : pin 7
 *     yellow LED  : pin 9
 *     red LED     : pin 11
 **/

const int LED[3] = {7, 9, 11};
const int debugLED = 13;
int analogValue = 0;
bool remoteIndicator = false;
bool lastRemoteIndicator = false;
unsigned long lastSent = 0;
int numBytes2Discard = 20;

void setup() {
  pinMode(debugLED, OUTPUT);
  for(int i=0; i<3; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
  }
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() >= 23) {
    // look for start byte
    if(Serial.read() == 0x7E) {
      digitalWrite(debugLED, HIGH);
      delay(10);
      digitalWrite(debugLED, LOW);

      for(int i=0; i<numBytes2Discard; i++) {
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
    remoteIndicator = false;
  }
  else if(analogValue <= 750) { // just right: LED[1] on
    which_led_on = 1;  // yellow led: just right
    remoteIndicator = true;
  }
  else { // too bright
    which_led_on = 2;  // red led: too bright
    remoteIndicator = false;
  }

  for(int i=0; i<3; i++) {
    digitalWrite(LED[i], i==which_led_on);
  }

  // set the indicator when there's a state change
  if(remoteIndicator != lastRemoteIndicator) {
    if(remoteIndicator) setRemoteStatus(0x05);
    else setRemoteStatus(0x04);
    lastRemoteIndicator = remoteIndicator;
    lastSent = millis();
  }

  // periodically set indicator in case it's out of sync
  if(millis() > lastSent + 250) {
    if(remoteIndicator) setRemoteStatus(0x05);
    else setRemoteStatus(0x04);
    lastSent = millis();
  }
}

// 0x4 = pin off
// 0x5 = pin on
void setRemoteStatus(int value) {
  long checksum;

  // start delimiter
  Serial.write(0x7E);

  // length
  Serial.write((byte)0); // high part of length
  Serial.write(0x10); // low part of length (note that 0x10 is hex for 16)

  // frame type: 0x17 = remote AT command
  Serial.write(0x17);
  checksum += 0x17;

  // frame id: 0 means no reply
  Serial.write((byte)0);

  // 64-bit destination address
  // 0x00 00 00 00 00 00 FF FF = broadcast
  for(int i=0; i<6; i++) {
    Serial.write((byte)0);
  }
  for(int i=0; i<2; i++) {
    Serial.write(0xFF);
    checksum += 0xFF;
  }

  // 16-bit destination address
  Serial.write(0xFF);
  Serial.write(0xFE);
  checksum += (0xFF + 0xFE);

  // 0x02 means apply changes immediately on remote
  Serial.write(0x02);
  checksum += 0x02;

  // command name in ASCII characters
  // D1 = set pin 1
  Serial.write('D');
  Serial.write('1');
  checksum += ('D' + '1');

  // turn on or off
  Serial.write(value);
  checksum += value;

  // checksum
  Serial.write(0xFF - (checksum & 0xFF));

  // don't overwhelm serial port
  delay(10);
}

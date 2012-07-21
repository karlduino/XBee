/**
 * XBee remote with photo sensor attached to pin 0 (analog input)
 *         and led to pin 1 (digital output)
 * XBee coordinator attached to 20x4 LCD display
 *
 * XBee code based on that from Robert Faludi, Building wireless
 * sensor networks, Chapter 4
 **/

#include <LiquidCrystal.h>
#include <Wire.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int debugLED = 13;
int analogValue = 0;
bool remoteIndicator = false;
bool lastRemoteIndicator = false;
unsigned long lastSent = 0;
int numBytes2Discard = 20;

void setup() {
  pinMode(debugLED, OUTPUT);
  Serial.begin(9600);

  // set up the LCD's number of rows and columns:
  lcd.begin(20, 4);
  lcd.setCursor(0, 1);
  lcd.print("analog value =");
}

void loop() {
  if(Serial.available() >= 23) {
    // look for start byte
    if(Serial.read() == 0x7E) {

      for(int i=0; i<numBytes2Discard; i++) {
        byte discard = Serial.read();
      }
      int analogHigh = Serial.read();
      int analogLow = Serial.read();
      analogValue = analogLow + (analogHigh * 256);

      lcd.setCursor(15,1);
      lcd.print("     ");
      lcd.setCursor(15, 1);
      if(analogValue < 1000) lcd.print(" ");
      if(analogValue < 100) lcd.print(" ");
      if(analogValue < 10) lcd.print(" ");
      lcd.print(analogValue);

      digitalWrite(debugLED, HIGH);
      delay(10);
      digitalWrite(debugLED, LOW);
    }
  }

  if(analogValue > 350 && analogValue <= 750) remoteIndicator = true;
  else remoteIndicator = false;

  // set the indicator when there's a state change
  if(remoteIndicator != lastRemoteIndicator || millis() > lastSent + 2000) {
    lcd.setCursor(0,3);
    if(remoteIndicator) {
      setRemoteStatus(0x05);
      lcd.print("remote LED on ");
    }
    else {
      setRemoteStatus(0x04);
      lcd.print("remote LED off");
    }
    lastRemoteIndicator = remoteIndicator;
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

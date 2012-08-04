/**
 * Garage door open/close indicator
 *
 * XBee remote with pins D0 and D1 set to digital input
 *         pin D0 attached to magnetic contact switch; HIGH when large garage door is open
 *         pin D1 attached to magnetic contact switch; HIGH when small garage door is open
 *         sample rate set at 2000 ms (ATIR7D0)
 *
 * XBee coordinator attached to arduino
 *         arduino digital pin 10: small door open
 *         arduino digital pin  9: small door shut
 *         arduino digital pin  6: large door open
 *         arduino digital pin  5: large door shut
 *         potentiometer attached to analog pin 0
 * 
 *
 * xbee-arduino library from https://code.google.com/p/xbee-arduino/
 **/

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

const int errorLED =    13;
const int smOpenLED =   10;
const int smClosedLED =  9;
const int lgOpenLED =    6;
const int lgClosedLED =  5;
const int potPin =       0;
const int buttonPin =   12;

unsigned int lgClosed=0, smClosed=0;
unsigned long lastRead = 0; 
unsigned int potSetting;
const int errorGap = 20000;

void setup() {
  xbee.begin(9600);

  pinMode(smOpenLED,  OUTPUT);
  pinMode(smClosedLED, OUTPUT);
  pinMode(lgOpenLED,  OUTPUT);
  pinMode(lgClosedLED, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(buttonPin, INPUT);

  // show that the LEDs are working
  flashLED(errorLED, 10, 50);
  flashLED(smOpenLED, 10, 50);
  flashLED(smClosedLED, 10, 50);
  flashLED(lgOpenLED, 10, 50);
  flashLED(lgClosedLED, 10, 50);
}

void loop() {
  // read potentiometer setting
  potSetting = analogRead(potPin);

  //attempt to read a packet
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) { // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      lgClosed = 1 - ioSample.isDigitalOn(0);
      smClosed = 1 - ioSample.isDigitalOn(1);
      lastRead = millis();

      if(lastRead + errorGap < lastRead) lastRead = 0; // roll over?
    }
    else {
      flashLED(errorLED, 3, 50);
    }
  } else if (xbee.getResponse().isError()) {
    flashLED(errorLED, 3, 50);
  }

  if(millis() > lastRead + errorGap) {  // haven't gotten signal in 20 seconds
    smClosed = lgClosed = 0;

    // error signal
    flashLED(errorLED, 1, 100);
    flashLED(smOpenLED, 1, 100);
    flashLED(smClosedLED, 1, 100);
    flashLED(lgOpenLED, 1, 100);
    flashLED(lgClosedLED, 1, 100);
  }
  else {

    if(smClosed) {
      digitalWrite(smOpenLED, LOW);
      analogWrite(smClosedLED, potSetting/4);
    } else {
      digitalWrite(smOpenLED, HIGH);
      digitalWrite(smClosedLED, LOW);
    }

    if(lgClosed) {
      digitalWrite(lgOpenLED, LOW);
      analogWrite(lgClosedLED, potSetting/4);
    } else {
      digitalWrite(lgOpenLED, HIGH);
      digitalWrite(lgClosedLED, LOW);
    }
  }
}

void flashLED(int pin, int number, int wait) {
  for(int i=0; i<number; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);
    if(i < number-1) delay(wait);
  }
}

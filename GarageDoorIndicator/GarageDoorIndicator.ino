/**
 * Garage door open/close indicator
 *
 * XBee remote with pins D0 and D1 set to digital input
 *         pin D0 attached to magnetic contact switch; HIGH when small garage door is shut
 *         pin D1 attached to magnetic contact switch; HIGH when large garage door is shut
 *
 * XBee coordinator attached to arduino
 *         arduino digital pin  4: small door shut
 *         arduino digital pin  6: small door open
 *         arduino digital pin  8: large door shut
 *         arduino digital pin 10: large door open
 *
 * xbee-arduino library from https://code.google.com/p/xbee-arduino/
 **/

#include <XBee.h>

XBee xbee = XBee();

ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

XBeeAddress64 remoteAddress64 = XBeeAddress64(0x0013a200, 0x40795C79);
uint16_t remoteAddress16 = 0;

const int smClosedLED = 4;
const int smOpenLED =   6;
const int lgClosedLED = 8;
const int lgOpenLED =   10;
const int errorLED = 13;

int lgClosed=0, smClosed=0;
unsigned long lastRead = 0;

void setup() {
  xbee.begin(9600);

  pinMode(smOpenLED,  OUTPUT);
  pinMode(smClosedLED, OUTPUT);
  pinMode(lgOpenLED,  OUTPUT);
  pinMode(lgClosedLED, OUTPUT);

  flashLED(errorLED, 5, 100);
  delay(500);
  flashLED(smClosedLED, 5, 100);
  delay(500);
  flashLED(smOpenLED, 5, 100);
  delay(500);
  flashLED(lgClosedLED, 5, 100);
  delay(500);
  flashLED(lgOpenLED, 5, 100);
}

void loop() {
  //attempt to read a packet
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      smClosed = ioSample.isDigitalOn(0);
      lgClosed = ioSample.isDigitalOn(1);
      lastRead = millis();
    }
    else {
      flashLED(errorLED, 3, 50);
    }
  } else if (xbee.getResponse().isError()) {
    flashLED(errorLED, 3, 50);
  }

  if(millis() > lastRead + 10000) {
    smClosed = lgClosed = 0;

    flashLED(errorLED, 1, 100);
    flashLED(smClosedLED, 1, 100);
    flashLED(smOpenLED, 1, 100);
    flashLED(lgClosedLED, 1, 100);
    flashLED(lgOpenLED, 1, 100);
  }
  else {

    if(smClosed) {
      digitalWrite(smOpenLED, LOW);
      digitalWrite(smClosedLED, HIGH);
    } else {
      digitalWrite(smOpenLED, HIGH);
      digitalWrite(smClosedLED, LOW);
    }

    if(lgClosed) {
      digitalWrite(lgOpenLED, LOW);
      digitalWrite(lgClosedLED, HIGH);
    } else {
      digitalWrite(lgOpenLED, HIGH);
      digitalWrite(lgClosedLED, LOW);
    }
  }
}

void flashLED(int pin, int numTimes, int delayAmount) {

  for(int i=0; i<numTimes; i++) {
    digitalWrite(pin, HIGH);
    delay(delayAmount);
    digitalWrite(pin, LOW);
    if(i < numTimes-1) delay(delayAmount);
  }
}

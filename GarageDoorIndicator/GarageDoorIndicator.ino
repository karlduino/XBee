/**
 * Garage door open/close indicator
 *
 * XBee remote with pins D0 and D1 set to digital input
 *         pin D0 attached to magnetic contact switch; HIGH when small garage door is shut
 *         pin D1 attached to magnetic contact switch; HIGH when large garage door is shut
 *         sample rate set at 2000 ms (ATIR7D0)
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

const int errorLED =    13;
const int smClosedLED =  4;
const int smOpenLED =    6;
const int lgClosedLED =  8;
const int lgOpenLED =   10;

int lgClosed=0, smClosed=0;
unsigned long lastRead = 0; 
const int errorGap = 20000;

void setup() {
  xbee.begin(9600);

  pinMode(smOpenLED,  OUTPUT);
  pinMode(smClosedLED, OUTPUT);
  pinMode(lgOpenLED,  OUTPUT);
  pinMode(lgClosedLED, OUTPUT);

  // show that the LEDs are working
  flashLED(errorLED, 10, 50);
  flashLED(smClosedLED, 10, 50);
  flashLED(smOpenLED, 10, 50);
  flashLED(lgClosedLED, 10, 50);
  flashLED(lgOpenLED, 10, 50);
}

void loop() {
  //attempt to read a packet
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) { // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      smClosed = ioSample.isDigitalOn(0);
      lgClosed = ioSample.isDigitalOn(1);
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

void flashLED(int pin, int number, int wait) {
  for(int i=0; i<number; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);
    if(i < number-1) delay(wait);
  }
}

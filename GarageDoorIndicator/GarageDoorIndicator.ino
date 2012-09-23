/**
 * Garage door open/close indicator
 *
 * XBee remote with pins D0 and D1 set to digital input
 *         pin D0 attached to magnetic contact switch
 *         pin D1 attached to magnetic contact switch
 *         sample rate set at 2000 ms (ATIR7D0)
 *
 * XBee coordinator attached to arduino
 *         arduino digital pin 10: small door open
 *         arduino digital pin  9: small door shut
 *         arduino digital pin  6: large door open
 *         arduino digital pin  5: large door shut
 *         potentiometer attached to analog pin 0
 *         photoresistor attached to analog pin 1
 *         pushbutton attached to digital pin 12
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
const int lightPin =     1;

unsigned int lgClosed=0, smClosed=0;
unsigned long lastRead = 0;
const int errorGap = 20000;

// use potentiometer or photosensor to set intensity of LED for "closed" indicators
unsigned int potSetting;
unsigned int lightSetting;
unsigned int smIntensity, lgIntensity;
unsigned int buttonState = 0, prevButtonState = 0;
unsigned int usePot = 0; // use pot (vs light sensor) to set green LED?

// conversion from potentiometer/photosensor inputs to led intensity output
const float maxPotValue = 1023;
const float maxLightValue = 650;

// lowest intensity for "closed" LED indicators
const int lowSmIntensity = 6, lowLgIntensity = 1;


void setup() {
  xbee.begin(9600);
  Serial.begin(9600);

  pinMode(smOpenLED,  OUTPUT);
  pinMode(smClosedLED, OUTPUT);
  pinMode(lgOpenLED,  OUTPUT);
  pinMode(lgClosedLED, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(lightPin, INPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH); // sets pull-up resistor; pressed with be LOW

  // show that the LEDs are working
  digitalWrite(errorLED, HIGH);
  digitalWrite(smOpenLED, HIGH);
  digitalWrite(smClosedLED, HIGH);
  digitalWrite(lgOpenLED, HIGH);
  digitalWrite(lgClosedLED, HIGH);

  delay(1500);

  digitalWrite(errorLED, LOW);
  digitalWrite(smOpenLED, LOW);
  digitalWrite(smClosedLED, LOW);
  digitalWrite(lgOpenLED, LOW);
  digitalWrite(lgClosedLED, LOW);
}

void loop() {
  // read potentiometer and photoresistor settings
  potSetting = analogRead(potPin);
  lightSetting = analogRead(lightPin);

  // check if button has been pressed; switches between use of pot and light sensor
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW && prevButtonState == HIGH) { // low = pressed; high = not
    usePot = 1 - usePot; delay(10);
    if(usePot) Serial.println("Use pot");
    else Serial.println("Use light sensor");
  }
  prevButtonState = buttonState;

  // convert reading to intensity for "closed" indicators
  if(usePot) {
    smIntensity = map(potSetting, 0, maxPotValue, lowSmIntensity, 255);
    lgIntensity = map(potSetting, 0, maxPotValue, lowLgIntensity, 255);
  } else {
    smIntensity = map(lightSetting, 0, maxLightValue, lowSmIntensity, 255);
    lgIntensity = map(lightSetting, 0, maxLightValue, lowLgIntensity, 255);
  }
  smIntensity = constrain(smIntensity, 1, 255);
  lgIntensity = constrain(lgIntensity, 1, 255);

  //attempt to read a packet
  xbee.readPacket();

  if(xbee.getResponse().isAvailable()) { // got something
    if(xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
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
    digitalWrite(errorLED, HIGH);
    digitalWrite(smOpenLED, HIGH);
    digitalWrite(smClosedLED, HIGH);
    digitalWrite(lgOpenLED, HIGH);
    digitalWrite(lgClosedLED, HIGH);
  }
  else {

    if(smClosed) {
      digitalWrite(smOpenLED, LOW);
      analogWrite(smClosedLED, smIntensity);
    } else {
      digitalWrite(smOpenLED, HIGH);
      digitalWrite(smClosedLED, LOW);
    }

    if(lgClosed) {
      digitalWrite(lgOpenLED, LOW);
      analogWrite(lgClosedLED, lgIntensity);
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

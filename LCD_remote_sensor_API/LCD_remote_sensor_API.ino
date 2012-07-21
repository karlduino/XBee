/**
 * XBee remote with photo sensor attached to pin 0 (analog input)
 *         and led to pin 1 (digital output)
 * XBee coordinator attached to 20x4 LCD display
 *
 * An attempt using the xbee-arduino library from https://code.google.com/p/xbee-arduino/
 **/

#include <XBee.h>
#include <LiquidCrystal.h>
#include <Wire.h>

XBee xbee = XBee();

ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

uint8_t d1Cmd[] = {'D', '1' };
uint8_t onValue[] = { 0x5 };
uint8_t offValue[] = { 0x4 };

XBeeAddress64 remoteAddress64 = XBeeAddress64(0x0013a200, 0x40795C79);
uint16_t remoteAddress16 = 0;
RemoteAtCommandRequest remoteLEDon = RemoteAtCommandRequest(0x000000000000FFFF, d1Cmd, onValue, sizeof(onValue));
RemoteAtCommandRequest remoteLEDoff = RemoteAtCommandRequest(0x000000000000FFFF, d1Cmd, offValue, sizeof(offValue));
RemoteAtCommandResponse remoteAtResponse = RemoteAtCommandResponse();

const int errorLED = 13;
int analogValue=0;
bool remoteIndicator = false;
bool lastRemoteIndicator = false;
unsigned long lastSent = 0;
bool address_set = false;

void setup() {
  xbee.begin(9600);
  lcd.begin(20, 4);
  pinMode(errorLED, OUTPUT);
  delay(2500);
  flashLED(errorLED, 3, 50);
  xbee.send(remoteLEDon);
  lcd.print("remote LED on");
  getResponse();
  delay(2500);
  xbee.send(remoteLEDoff);
  lcd.home();
  lcd.print("remote LED off");
  getResponse();
  delay(2500);
  lcd.clear();
}

void loop() {


  //attempt to read a packet
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      // read analog inputs
      lcd.setCursor(0, 0);
      lcd.print("photosensor = ");
      lcd.setCursor(15, 0);
      lcd.print("    ");
      lcd.setCursor(15, 0);
      analogValue = ioSample.getAnalog(0);
      lcd.print(analogValue);

    }
    else {
      flashLED(errorLED, 3, 50);
    }
  } else if (xbee.getResponse().isError()) {
    flashLED(errorLED, 3, 50);
  }

  if(analogValue < 500 ) remoteIndicator = true;
  else remoteIndicator = false;

  // set the indicator when there's a state change
  if(remoteIndicator != lastRemoteIndicator || millis() > lastSent + 2500) {
    lcd.setCursor(0,1);
    if(remoteIndicator) {
      xbee.send(remoteLEDon);
      lcd.print("remote LED on ");
    }
    else {
      xbee.send(remoteLEDoff);
      lcd.print("remote LED off");
    }

    getResponse();

    lastRemoteIndicator = remoteIndicator;
    lastSent = millis();
  }

  if(remoteAddress16) {
    remoteLEDon.setRemoteAddress16(remoteAddress16);
    remoteLEDoff.setRemoteAddress16(remoteAddress16);
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

void getResponse(void) {
  if(xbee.readPacket(5000)) {
    if (xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
      xbee.getResponse().getRemoteAtCommandResponse(remoteAtResponse);
      remoteAddress64 = remoteAtResponse.getRemoteAddress64();
      remoteAddress16 = remoteAtResponse.getRemoteAddress16();
      lcd.setCursor(0, 3);
      lcd.print(remoteAddress64.getMsb(), HEX);
      lcd.setCursor(9, 3);
      lcd.print(remoteAddress64.getLsb(), HEX);
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(0, 2);
      lcd.print(remoteAddress16, HEX);

      if(remoteAtResponse.isError()) {
        flashLED(errorLED, 3, 50);
      }
    }
  }
}

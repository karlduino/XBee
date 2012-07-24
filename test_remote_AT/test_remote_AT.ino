
#include <XBee.h>
#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

XBee xbee = XBee();
// Set D1 (pin 19) to HIGH
uint8_t d1Cmd[] = { 'D', '1' };
uint8_t d1ValueOn[] = { 0x5 };
uint8_t d1ValueOff[] = { 0x4 };

const int errorLED = 13;

// SH + SL of the XbeeB
XBeeAddress64 remoteAddress64;
uint16_t remoteAddress16 = 0x000000000000FFFF;
bool address_set = false;
                                              // Create a remote AT request with the IR command
RemoteAtCommandRequest remoteLEDon = RemoteAtCommandRequest(remoteAddress16, d1Cmd, d1ValueOn, sizeof(d1ValueOn));
RemoteAtCommandRequest remoteLEDoff = RemoteAtCommandRequest(remoteAddress16, d1Cmd, d1ValueOff, sizeof(d1ValueOff));
RemoteAtCommandResponse remoteAtResponse = RemoteAtCommandResponse();

void setup() {
  xbee.begin(9600);
  lcd.begin(20, 4);
  pinMode(errorLED, OUTPUT);
  flashLED(errorLED, 3, 50);

  //time to build the communication
  delay(2000);
}

void loop() {
  xbee.send(remoteLEDon);
  lcd.home();
  lcd.print("On ");
  getResponse();
  delay(2000);

  xbee.send(remoteLEDoff);
  lcd.home();
  lcd.print("Off");
  getResponse();
  delay(2000);
}

void getResponse(void) {

  if (xbee.readPacket(5000)) {

    // should be an AT command response
    if(xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
      xbee.getResponse().getRemoteAtCommandResponse(remoteAtResponse);

      remoteAddress64 = remoteAtResponse.getRemoteAddress64();
      lcd.setCursor(0, 3);
      lcd.print(remoteAddress64.getMsb(), HEX);
      lcd.setCursor(9, 3);
      lcd.print(remoteAddress64.getLsb(), HEX);

      /*
      if(!address_set) {
        remoteLEDon.setRemoteAddress64(remoteAddress64);
        remoteLEDoff.setRemoteAddress64(remoteAddress64);
        address_set = true;
        lcd.setCursor(15, 2);
        lcd.print("set");
      }
      */

      if (remoteAtResponse.isOk()) {
        if (remoteAtResponse.getValueLength() > 0) {
        }
      } else {
      }
    } else {
    }
  } else if (xbee.getResponse().isError()) {
    flashLED(errorLED, 3, 50);
  } else {
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

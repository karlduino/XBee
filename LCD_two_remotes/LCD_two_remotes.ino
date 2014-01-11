/**
 * Two XBee remotes with LEDs attached to pin 1 (digital output)
 * XBee coordinator attached to 20x4 LCD display
 *
 * An attempt using the xbee-arduino library from https://code.google.com/p/xbee-arduino/
 **/

#include <XBee.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

XBee xbee = XBee();
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// background color
#define TEAL 0x6

uint8_t d0Cmd[] = {'D', '0' };
uint8_t onValue[] = { 0x5 };
uint8_t offValue[] = { 0x4 };

XBeeAddress64 remote64[2] = { XBeeAddress64(0x0013a200, 0x4092d672),
                              XBeeAddress64(0x0013a200, 0x4092d7d8) };
XBeeAddress64 tmpRemote64;
uint16_t remote16[2] = { 0x0, 0xEF5C };
uint16_t tmpRemote16;
RemoteAtCommandRequest remoteCommand[2];
RemoteAtCommandResponse remoteResponse = RemoteAtCommandResponse();

const int errorLED = 13;
bool turn_on[2] = { false, false };
int which_xbee_resp = 0;
int which_xbee_cmd = 1;

void setup() {
  xbee.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(TEAL);

  for(int i=0; i<2; i++) {
    //    remoteCommand[i] = RemoteAtCommandRequest(remote64[i], d0Cmd, onValue, sizeof(onValue));
    remoteCommand[i] = RemoteAtCommandRequest(remote16[i], d0Cmd, onValue, sizeof(onValue));
    //    remoteCommand[i].setRemoteAddress64(remote64[i]);

    lcd.setCursor(0, i);
    lcd.print("remote ");
    lcd.print(i);
    lcd.print(": on ");
    xbee.send(remoteCommand[i]);
  }
  flashLED(errorLED, 3, 30);
  delay(2000);


  /*
  getResponse();
  flashLED(errorLED, 3, 30);
  delay(1000);

  getResponse();
  flashLED(errorLED, 3, 30);
  delay(1000);
  */
}

void loop() {
  which_xbee_cmd = 1 - which_xbee_cmd;

  //  remoteCommand.setRemoteAddress16(remote16[which_xbee_cmd]);

  if(turn_on[which_xbee_cmd] == true) {
    turn_on[which_xbee_cmd] = false;
    remoteCommand[which_xbee_cmd].setCommandValue(onValue);
    lcd.setCursor(10, which_xbee_cmd);
    lcd.print("on ");
  }
  else {
    turn_on[which_xbee_cmd] = true;
    remoteCommand[which_xbee_cmd].setCommandValue(offValue);
    lcd.setCursor(10, which_xbee_cmd);
    lcd.print("off");
  }

  lcd.setCursor(0, 2);
  lcd.print(remoteCommand[which_xbee_cmd].getRemoteAddress16(), HEX);

  xbee.send(remoteCommand[which_xbee_cmd]);
  flashLED(errorLED, 3, 30);
  delay(2000);

  /*
  getResponse();
  flashLED(errorLED, 3, 30);

  getResponse();
  flashLED(errorLED, 3, 30);
  */
}

void flashLED(int pin, int number, int wait) {
  for(int i=0; i<number; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);
    if(i < number-1) delay(wait);
  }
}

void getResponse(void) {
  if(xbee.readPacket(1000)) {
    if (xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
      xbee.getResponse().getRemoteAtCommandResponse(remoteResponse);
      tmpRemote64 = remoteResponse.getRemoteAddress64();
      tmpRemote16 = remoteResponse.getRemoteAddress16();
      for(int i=0; i<2; i++) {
        if(tmpRemote64.getLsb() == remote64[i].getLsb()) {
          which_xbee_resp = i;
          remote16[which_xbee_resp] = tmpRemote16;
        }
      }

      lcd.setCursor(0, which_xbee_resp+2);
      lcd.print(remote64[which_xbee_resp].getLsb(), HEX);
      lcd.setCursor(9, which_xbee_resp+2);
      lcd.print(remote16[which_xbee_resp], HEX);

      if(remoteResponse.isError()) {
        flashLED(errorLED, 3, 50);
      }
    }
  }
}

== XBees Button & Light

This is a simple test of using two XBees to communicate between two
Arduinos.

Each XBee (mine are Series 2) are in AT mode. Each is connected to an
Arduino Uno using a Sparkfun XBee Shield.

The first one (with XBee as Coordinator AT) has a button connected to
digital pin 2 on the Arduino. I'm using the internal pull-up resistor,
so the connection is just pin 2 ----- button ---- ground.  This one
gets the XBeeButton sketch.

The second one (with XBee as Router AT) has an LED connected to pin
13. (Or just use the built-in one, but it's hidden by the shield.
This one gets the XBeeLight sketch.

When you press the button, the first arduino sends a `P` to the other
one. The other one turns on the LED whenever a `P` is received and
turns it off otherwise.

Timing seems important. The first Arduino delays 10 ms between
messages; the second delays 10 ms after turning the LED on and then
turns it off.


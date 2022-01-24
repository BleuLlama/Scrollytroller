# Scrollytroller
Scrollytroller is a crank-based USB controller, initially inteneded for use with Playdate's Pulp web-based game development tool.

<img src="images/Controller.jpg">

This is a USB controller that lets you try out your Playdate Pulp games with a more accurate 
user interface, compared with using your keyboard.

# Function
The controller generates keyboard presses and mouse scroll wheel events so that the 
Pulp web player will interact with it properly.  (I added more buttons and the LEDs because I 
had the space and thought they might be useful.)

The current mouse+keyboard configuration is:

- Crank generates mouse scroll wheel events
- D-Pad generates keyboard arrow key press events
- A and B buttons - generate keyboard 'a' and 's' events
- C button (in the middle) - not used 
- D and E bottom buttons - generate 'cmd'-'s' to save and 'cmd'-'b' to toggle between the player and the editor

In general use, just the green LED will be lit.
You can switch it into "joystick" mode by pushing in the crank button.  The red LED will light up.
While in this config mode, you can turn the crank to reset it, to match the web player.
You can also press the A and B buttons to switch output modes.  The green LED should be off after
you first power up the unit.  This indicates that you're in keyboard+mouse mode.  Press the B button
to switch to Joystick mode.  The green LED will be lit.

The current joystick configuration is:

- D-Pad generates output on axis 0 (x) and axis 1 (y)
 - Range sent is from -1 to +1, with 0 being the "middle"
- A,B,D,E buttons will generate presses on buttons 0,1,2,3 respectively
 - These send 0 when idle, 1, when pressed
- Crank will output on axis 2, and the range of the axis corresponds to one full revolution of the crank.
 - This sends a value from 0..360, and loops around when a full revolution occurs in either direction

Note: The code defines the full revolution based on a couple factors that are specific to the rotary encoder that I'm using.  Mine has 20 detents per full revolution, and each detent sends a cycle of all 4 gray-code encoder phases.

# Arduino Code

The code is written with the Arduino tools, and requires the Encoder library from PJRC, and the
joystick library from Matthew Heironimus. The urls for these libraries can be found in the source
code, and may be able to be installed through the Arduino Library manager.

This project requires an Arduino board with an ATmega 32u4 chip - Leonardo, Pro Micro, or in
my case, I used the "SS Micro" clone board because I like the formfactor.

<img src="images/Internals.jpg">

The connection details are in the Arduino source code.  But the short of it is that 
all button io is configured using the 32u4's internal pullups, so none are needed
in the external wiring.  All connections go directly to the buttons.  The "other pin" 
on the buttons go to a common ground.  LEDs are wired up in a "source" configuration
where a HIGH from the Arduino will turn the LEDs on.  I used a 510 ohm resistor on each
to keep the current (and brightness) down quite a bit.

# Printable Model

The 3D model started out as the Adafruit "[USB Crank Controller](https://www.thingiverse.com/thing:3685130)" model on Thingiverse.  I widened
it, and created the D-Pad and buttons, as well as mounting standoffs so that it can
be screwed together.  

My version, seen above, started with that model, but I took it in [Tinkercad](https://tinkercad.com), chopped it up, and made a squareish version, with space for a D-Pad and buttons

# Links
- [Playdate](https://play.date)
- [Playdate Pulp](https://play.date/pulp)
- [Scrollytroller on Thingiverse](https://www.thingiverse.com/thing:5215011)
- [Adafruit's USB Crank Controller](https://www.thingiverse.com/thing:3685130)
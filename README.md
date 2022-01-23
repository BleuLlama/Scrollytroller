# Scrollytroller
A crank-based controller for use with PlayDate's Pulp web player development tool

<img src="images/Controller.jpg">

This is a USB controller that lets you try out your PlayDate Pulp games with a more accurate 
user interface, compared with using your keyboard.

The code is written with the Arduino tools, and requires the Encoder library (url is in
the source code) to handle all of the interrupt-driven rotary encoder stuff.

This requires an Arduino board with an ATmega 32u4 chip - Leonardo, Pro Micro, or in
my case, I used the "SS Micro" clone board because I like the formfactor.

<img src="images/Internals.jpg">

The connection details are in the Arduino source code.  But the short of it is that 
all button io is configured using the 32u4's internal pullups, so none are needed
in the external wiring.  All connections go directly to the buttons.  The "other pin" 
on the buttons go to a common ground.  LEDs are wired up in a "source" configuration
where a HIGH from the Arduino will turn the LEDs on.  I used a 510 ohm resistor on each
to keep the current (and brightness) down quite a bit.

The 3D model started out as the Adafruit "Crankbox" model on Thingiverse.  I widened
it, and created the D-Pad and buttons, as well as mounting standoffs so that it can
be screwed together.  My printed device is all my parts, other than the assembled-when-printed
crank assembly.

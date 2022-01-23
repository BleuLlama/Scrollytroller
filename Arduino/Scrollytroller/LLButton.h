#ifndef __LLBUTTON_H__
#define __LLBUTTON_H__


// Button events
// the Poll() routine will return one of these to indicate the current
// state of the button
#define kEventIdle      (0x00)  // button is unpressed (will repeat until pressed)
#define kEventPressed   (0x11)  // button was JUST pressed (only one of these per press cycle)
#define kEventPressing  (0x10)  // user is pressing the button now (will repeat until released)
#define kEventReleased  (0x21)  // button was JUST released (onlu one of these per press cycle)


////////////////////////////////////////////////
// LLButton
//
//  a class to encapsulate the actions of a button attached to an arduino pin
//  buttons in this implementation are configured as INPUT_PULLUP, and thus
//  require no external pullups or pulldowns.  Buttons should short the data pin
//  to ground when a "press" occurs

class LLButton {
protected:
  int prev; // previous pin state
  int curr; // current pin state
  int pin;  // specified gpio pin
  int event; // most recent generated button event (see above)
  
public:
  LLButton( int pin );  // constructor - pass in the gpio pin specifier ie 8, 9, A3, etc
  int Poll();           // called to check the states of the buttons
  int LastEvent();      // just return the last event that Poll() returned
};


////////////////////////////////////////////////
// LLButtonKeyboard
//
//  This is a subclass of the above. It adds in keyboard functionality
//  that is, when the gpio pin is "pressed" it will send a keyboard key-down event,
//  with the specified key.  (and wrapped in the modifier if appropriate)

class LLButtonKeyboard : public LLButton {
  private:
    int key;      // keyboard key to send (ascii 0x00..0x7f)
    int modifier; // modifier to press with it (ie "command", "alt" etc)
  public:
    LLButtonKeyboard( int pin, int key );               // constructor with the key passed in (no modifier)
    LLButtonKeyboard( int pin, int key, int modifier ); // constructor with the key and modifier passed in
    int Poll(); // poll the button pin and generate key presses where appropriate
};

#endif

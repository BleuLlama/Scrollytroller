/*  LLButton
 *   2022-01-22 - Scott Lawrence - yorgle@gmail.com
 *   
 *   Classes to encapsulate gpio -> button events and gpio -> keypesses
 */



#include "LLButton.h"

////////////////////////////////////////////////
// LLButton
//
//  a class to encapsulate the actions of a button attached to an arduino pin
//  buttons in this implementation are configured as INPUT_PULLUP, and thus
//  require no external pullups or pulldowns.  Buttons should short the data pin
//  to ground when a "press" occurs

// constructor
//   pass in the gpio pin specifier ie 8, 9, A3, etc
LLButton::LLButton( int ppin )
  : prev( HIGH )
  , curr( HIGH )
  , pin( ppin )
  , event( kEventIdle )
{
  // initialize the GPIO
  pinMode( this->pin, INPUT_PULLUP );
}


// LastEvent
//  just return the last event that Poll() returned
int LLButton::LastEvent()
{
  return this->event;
}


// Poll
//  called to check the states of the buttons, set the event state
int LLButton::Poll()
{
  int raw = digitalRead( this->pin );
  if( raw != prev ) {
    // state changed!
    if( prev == HIGH ) {
      this->event = kEventPressed;
    } else {
      this->event = kEventReleased;
    }
    prev = raw;
  } else {
    // no change
    if( raw == HIGH ) {
      this->event = kEventIdle;
    } else {
      this->event = kEventPressing;
    }
  }
  return this->event;
}


////////////////////////////////////////////////
// LLButtonKeyboard
//
//  This is a subclass of the above. It adds in keyboard functionality
//  that is, when the gpio pin is "pressed" it will send a keyboard key-down event,
//  with the specified key.  (and wrapped in the modifier if appropriate)

// constructor with the key passed in (no modifier)
LLButtonKeyboard::LLButtonKeyboard( int pin, int pkey )
  : LLButton( pin )
  , key( pkey )
  , modifier( -1 ) // indicate "no modifier"
{
}

// constructor with the key and modifier passed in
LLButtonKeyboard::LLButtonKeyboard( int pin, int pkey, int pmodifier )
  : LLButton( pin )
  , key( pkey )
  , modifier( pmodifier )
{
}

// Poll
//  // poll the button pin and generate key presses where appropriate
int LLButtonKeyboard::Poll()
{
  int evt = LLButton::Poll();

  if( evt == kEventPressed ) {
    if( this->modifier > 0) Keyboard.press( this->modifier );
    Keyboard.press( this->key );
  }
  if( evt == kEventReleased ) {
    if( this->modifier > 0 ) Keyboard.release( this->modifier );
    Keyboard.release( this->key ); 
  }
  
  return evt;
}

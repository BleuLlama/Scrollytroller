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
  , key2( 0 )
  , modifier( -1 ) // indicate "no modifier"
{
}

// constructor with the key and modifier passed in
LLButtonKeyboard::LLButtonKeyboard( int pin, int pkey, int pmodifier )
  : LLButton( pin )
  , key( pkey )
  , key2( 0 )
  , modifier( pmodifier )
{
}

// constructor with the key and modifier passed in
LLButtonKeyboard::LLButtonKeyboard( int pin, int pkey, int pmodifier, int pkey2 )
  : LLButton( pin )
  , key( pkey )
  , key2( pkey2 )
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

    if( this->key2 != 0 ) {
      delay( 3000 );
      Keyboard.press( this->key2 );
      delay( 250 );
      Keyboard.release( this->key2 );
    }
  }
  
  return evt;
}



////////////////////////////////////////////////
// LLButtonJoystick
// 
//  Subclass of the above, generates X/Y axis joystick controller events

// constructor with the key passed in (no modifier)
LLButtonJoystick::LLButtonJoystick( int pin, int xMul, int yMul  )
  : LLButton( pin )
  , xMultiplier( xMul )
  , yMultiplier( yMul )
{
}

int LLButtonJoystick::Poll()
{
  int evt = LLButton::Poll();

  if( evt == kEventPressed ) {
    // set the value on the appropriate axis
    if( this->xMultiplier != 0 ) {
      Joystick.setXAxis( this->xMultiplier );
    } else {
      Joystick.setYAxis( this->yMultiplier );
    }
    
  }
  if( evt == kEventReleased ) {
    // center the appropriate axis
    if( this->xMultiplier != 0 ) {
      Joystick.setXAxis( 0 );
    } else {
      Joystick.setYAxis( 0 );
    }
    
  }
  
  return evt;
}


////////////////////////////////////////////////
// LLButtonJoyButton
// 
//  Subclass of the above, generates joystick controller button events


LLButtonJoyButton::LLButtonJoyButton( int pin, int _joybutton )
: LLButton( pin )
  , joybutton( _joybutton )
{
}

int LLButtonJoyButton::Poll()
{
  int evt = LLButton::Poll();
  
  if( evt == kEventPressed ) {
    // set the value on the appropriate axis
    Joystick.setButton( this->joybutton, 1 );
  }
  if( evt == kEventReleased ) {
    Joystick.setButton( this->joybutton, 0 );
  }

  return evt;
}

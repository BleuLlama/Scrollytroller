/*
 * Scrollytroller
 *    Scott Lawrence yorgle@gmail.com
 * 
 * Scroll controler, made specifically for working as a controller for the PlayDate Pulp simulator
 * 
 * crank handle sends mouse scroll events
 * buttons will send key presses
 */

/* 
  v1.02 - 2022-01-24 - Added second keypress with 3s delay (for cmd-b, f)
  v1.01 - 2022-01-23 - Two modes - Joystick, KB+Mouse
  v1.00 - 2022-01-22 - Initial version - KB+Mouse
 */

////////////////////////////////////////////////

#include <Keyboard.h> // for keyboard output
#include <Mouse.h>    // for scroll wheel
#include <Encoder.h>  // this library https://www.pjrc.com/teensy/td_libs_Encoder.html
#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary


#include "LLButton.h" // my classes for handling button press and keyboard sending

////////////////////////////////////////////////
// pin config 

/*
   Controller Layout:

                (GRN) (RED)

        UP         B   A     (CRANK)
    LEFT  RIGHT      C
       DOWN        E   D

  keyboard+mouse mode mapping:
   CRANK - Mouse Scroll wheel
   UP/DOWN/LEFT/RIGHT - keyboard cursor keys
   A - keyboard 's' key
   B - keyboard 'a' key
   C - (not used)
   D - Command-S (save)
   E - Command-B (toggle between Player and Editor)
   
  joystick mode mapping:
   D-Pad - x, y axes (axis 0, axis 1)
   Crank - z axis (0..360)
   A - Button 0
   B - Button 1
   C - Button 2
   D - Button 3
   
   GRN - green LED
   RED - red LED

   press the crank button and then A to switch to keyboard mode, or B to joystick mode

   
   SS Micro / Arduino Leonardo-clone (32u4) pinout
 
  rBT  D16                          A2   B-B
  GRN  D15                          A1   B-A
  RED  D14                          A0   RT
       RST                          D9   B-D
       5v                           D10  B-E
       GND                          D11  LT
           3V3  D3  D2  D1  D0  D6
                rB  rA  UP  B-C DN

 Note: on my device, D0 seems to be faulty. I could not use it for "Right dpad input" so i swapped it 
       with button C, the center button on my controller.
 */

// rotary encoder controller
#define kRotaryA (2)
#define kRotaryB (3)
#define kRotaryButton (16)

// D-Pad
#define kPadUp (1)
#define kPadDown (6)
#define kPadLeft (11)
#define kPadRight (A0)

// Buttons
#define kButtonA  (A1)
#define kButtonB  (A2)
//#define kButtonC  (0) // skipped
#define kButtonD  (9)
#define kButtonE  (10)

#define kControlCount (8) // 4 dpad. 4 buttons

// LEDs
#define kLED_Red (14)
#define kLED_Green (15)

// settings
#undef kInvertEncoder

#define kTicksPerRevolution   (20 * 4)

int outputModeJoystick = 0;
int outputInhibit = 0;


////////////////////////////////////////////////
// objects

/// setup the encoder 
#ifdef kInvertEncoder
Encoder myEnc(kRotaryB, kRotaryA);
#else
Encoder myEnc(kRotaryA, kRotaryB);
#endif


/// init button objects
LLButton rotButton( kRotaryButton );
LLButtonKeyboard *controls[ kControlCount ];

LLButtonJoystick * jcontrols[ 4 ];
LLButtonJoyButton * jbcontrols[ 4 ];


// joystick 
Joystick_ Joystick(
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_JOYSTICK, //JOYSTICK_TYPE_MULTI_AXIS,
  4, 0,                  // Button Count, Hat Switch Count
  true, true, true,     // X and Y, and Z Axis
  false, false, false,    //  Rx, Ry, or Rz
  false, false,           // rudder or throttle
  false, false, false);   // accelerator, brake, or steering


////////////////////////////////////////////////
// Arduino core functions

// setup
//  standard setup() call.  get everything going!
void setup() {
  /*
  Serial.begin( 115200 );
  while( !Serial );
  Serial.println( "Ready." );
  */
  
  Mouse.begin();
  Keyboard.begin();

  setup_Joystick();
 
  setup_Buttons();
  setup_LEDs();

}

// loop
//  poll all of our inputs, generating appropriate events and stuff
void loop() {
  poll_Encoder();
  if( !outputInhibit ) {
    poll_Buttons();
  }
}

////////////////////////////////////////////////
// Joystick
void setup_Joystick()
{
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
  Joystick.setZAxisRange(0, 360);

  Joystick.setButton(0, 0); // A 
  Joystick.setButton(1, 0); // B
  Joystick.setButton(2, 0); // D
  Joystick.setButton(3, 0); // E
}

////////////////////////////////////////////////
// Buttons

//  setup_Buttons
//    Initialize all of our "controls" class instatiation thingies
void setup_Buttons()
{  
  // these map to the Pulp player controller
  // joystick controls
  jcontrols[0] = new LLButtonJoystick( kPadUp, 0, 1 );
  jcontrols[1] = new LLButtonJoystick( kPadDown, 0, -1 );
  jcontrols[2] = new LLButtonJoystick( kPadLeft, -1, 0 );
  jcontrols[3] = new LLButtonJoystick( kPadRight, 1, 0 );
  
  jbcontrols[0] = new LLButtonJoyButton( kButtonA, 0 );
  jbcontrols[1] = new LLButtonJoyButton( kButtonB, 1 );
  jbcontrols[2] = new LLButtonJoyButton( kButtonD, 2 );
  jbcontrols[3] = new LLButtonJoyButton( kButtonE, 3 );

  // kyb/mouse controls
  controls[0] = new LLButtonKeyboard( kPadUp, KEY_UP_ARROW );
  controls[1] = new LLButtonKeyboard( kPadDown, KEY_DOWN_ARROW );
  controls[2] = new LLButtonKeyboard( kPadLeft, KEY_LEFT_ARROW );
  controls[3] = new LLButtonKeyboard( kPadRight, KEY_RIGHT_ARROW );
  controls[4] = new LLButtonKeyboard( kButtonA, 's' );
  controls[5] = new LLButtonKeyboard( kButtonB, 'a' );

  controls[6] = new LLButtonKeyboard( kButtonD, 's', KEY_LEFT_GUI ); // save
  controls[7] = new LLButtonKeyboard( kButtonE, 'b', KEY_LEFT_GUI, 'f' ); // toggle between player and editor
}

//  poll_Buttons
//    poll each button to generate appropriate events.
//
void poll_Buttons()
{
  if( outputModeJoystick ) {
    for( int j = 0 ; j < 4 ; j++ ) {
      jcontrols[ j ]->Poll();
      jbcontrols[ j ]->Poll();
    }
  } else { 
    for( int i = 0 ; i < kControlCount ; i++ ) {
      controls[i]->Poll();
    }
  }
}


////////////////////////////////////////////////
// LEDs 

//  setup_LEDs
//    Setup pins for the LEDs and get them in a known state
void setup_LEDs()
{
  pinMode( kLED_Red, OUTPUT );
  pinMode( kLED_Green, OUTPUT );

  // flash them on, at poweron just to indicate they both work
  digitalWrite( kLED_Red, HIGH);
  digitalWrite( kLED_Green, HIGH );
  delay( 200 );
  digitalWrite( kLED_Red, LOW );
  digitalWrite( kLED_Green, LOW );
  // and start with them in a known off state

  // now indicate "ready"
  LED_Ready();
}


// LED_Ready
//  Indicate the "ready" state
void LED_Ready()
{
  // ready: red off, green on
  digitalWrite( kLED_Red, LOW );
  digitalWrite( kLED_Green, HIGH );
}


// LED_Ready
//  Indicate an "error" or "busy" state
void LED_Error()
{
  // error: red on, green off
  digitalWrite( kLED_Red, HIGH );
  digitalWrite( kLED_Green, LOW );
}


////////////////////////////////////////////////
// Rotary Encoder

// poll_Encoder
//  read the encoder and send mouse scroll events
void poll_Encoder()
{
  // states from call-to-call
  static long oldPosition = -999;
  long newPosition = 0;

  // for now, press the encoder button to "hold" rotational reporting, 
  // so that we can line up the arm with the simulator's arm.
  int bEvent = rotButton.Poll();
  switch( bEvent ) {
    case kEventPressed:
      LED_Error(); // indicate that something is different.
      outputInhibit = 1;
      
      // setup the LEDs for showing mode
      // red indicates we're inhibiting output
      digitalWrite( kLED_Red, HIGH );
      // green indicates keyboard or joystick
      digitalWrite( kLED_Green, outputModeJoystick );
      break;
      
    case kEventPressing:
      // eat rotation deltas
      myEnc.write( 0 );
      oldPosition = 0;
      outputInhibit = 1;
      // check for Button A/B press during this time
      
      if( controls[4]->Poll() == kEventPressed ) { // B button
        outputModeJoystick = 0;
      }
      if( controls[5]->Poll() == kEventPressed ) { // A button
        outputModeJoystick = 1;
      }
      digitalWrite( kLED_Green, outputModeJoystick );
      
      break;
      
    case kEventReleased:
      outputInhibit = 0;
      LED_Ready(); // back to normal!
      break;
      
    default: // or idle
      // user isn't pressing, send them events through!
      outputInhibit = 0;
      
      // checks for the encoder itself
      newPosition = myEnc.read();

      if( outputModeJoystick ) {
        if( newPosition < 0 ) {
          newPosition += kTicksPerRevolution;
          
          myEnc.write( newPosition );
        }
        
        Joystick.setZAxis( ( newPosition % kTicksPerRevolution ) * 360.0 / kTicksPerRevolution );
        
      } else {
          
        // since each "click" of the encoder goes through all four complete phases on our 
        // encoder... 00 -> 01 -> 11 -> 10 -> 00  (or the other way around), we want to only
        // respond for each "click" so divide the pulses by 4.
        
        if (newPosition/4 != oldPosition/4) {
          oldPosition = newPosition;
          Mouse.move( 0, 0, newPosition/4 );
          myEnc.write( 0 ); // reset it so we're doing a delta
        }   
      }
      break;
  }
}

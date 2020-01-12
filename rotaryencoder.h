/* This software decodes the output from a rotary encoder.
   Slightly reworked code by lingib from original file name: rotary_encoder_1.ino
   https://www.instructables.com/member/lingib/instructables/
 
  The count changes whenever there is a transition on ContactA.
  Contact bounce is eliminated using a pair of integrators.
  The direction is determined by ContactB
   - The logic states of ContactA and ContactB are opposite for clockwise rotation.
   - The logic states of ContactA and ContactB are the same for counter clockwise rotation.

  Since the output waveforms from ContactA and ContactB are displaced by 90 degrees,
  the raw count changes by +/-2 between indents. The reason for this is that there
  are always two positions between adjacent detents for which ContactA and ContactB
  have the same logic state. The count is corrected using "modulo 2" arithmetic.

  Direction is determined as follows.

  The encoder terminals are both high when resting on a "detent".
  Whenever there is a change in the state of ContactA:
   - the logic state of ContactB is always "opposite" to ContactA for clockwise rotation.
   - the logic state of ContactB is always the "same" as ContactA for counterclockwise.

  ----------
  COPYRIGHT
  ----------
  This code is free software: you can redistribute it and/or
  modify it under the terms of the GNU General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License. If
  not, see <http://www.gnu.org/licenses/>.
*/

// ----- Logic
boolean LastStateA;
boolean CurrentStateA;
boolean CurrentStateB;

// ----- Counter
int Count = 0;
int LastCount = Count;

boolean stateContactA() 
{/*  
     Two integrators are used to suppress contact bounce.
     The first integrator to reach MaxCount wins        
  */
  int Closed = 0;                        // Integrator
  int Open = 0;                          // Integrator
  int MaxCount = 250;                    // Increase this value if you see contact bounce

  // ----- Debounce Contact A
  while (1) {
    // ----- Check ContactA
    if (digitalRead(ContactA)) {
      // ----- ContactA is Open
      Closed = 0;                        // Empty opposite integrator
      Open++;                            // Integrate
      if (Open > MaxCount) return HIGH;
    } else {
      // ----- ContactA is Closed
      Open = 0;                          // Empty opposite integrator
      Closed++;                          // Integrate
      if (Closed > MaxCount) return LOW;
    }
  }
}

bool checkEncoder() 
{/*
     The encoder output comprises two 90 degree offset squarewaves.
     The direction of rotation is:
     - clockwise if the state of output A is opposite to output B
     - counterclockwise if the state of output A is the same as output B
  */
  CurrentStateA = stateContactA(); // Read rotary encoder

  // ----- Record changes
  if (CurrentStateA != LastStateA) {
    CurrentStateB = digitalRead(ContactB);         // Check state of ContactB
    if (CurrentStateA == CurrentStateB) Count++;   // Clockwise rotation
    if (CurrentStateA != CurrentStateB) Count--;   // Counterclockwise rotation
    LastStateA = CurrentStateA;
  }
  if (Count != LastCount) {
    LastCount = Count;
    /* There are two transitions between encoder detents.
       This causes the "Count" to increment twice between indent clicks. */  
    if (Count % 2 == 0) {
      rotaryencoder = Count; 
      #if DEBUG >= 1
        // Display the number of "detents"
        Serial.print("Count: ");
        Serial.println(Count / 2);
      #endif      
      return true; 
    }    
  } else {
    return false;
  }
}

void initrotary() 
{/* ----- Configure encoder ----- */
  pinMode(ContactA, INPUT_PULLUP);
  pinMode(ContactB, INPUT_PULLUP);
  LastStateA = stateContactA();
}

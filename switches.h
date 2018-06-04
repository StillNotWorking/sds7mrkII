/* Simmons SDS7 mrk-II w/m2560
 * Switch and selector pad control
 * 
 * keypad state logic are reworked code from 
 * Keypad library https://github.com/Chris--A/Keypad
 * 
 */

uint32_t startA=0; uint32_t stopA=0; //temp variables for code testing

#define BIT_READ(value, bit) (((value) >> (bit)) & 0x01)
#define BIT_SET(value, bit) ((value) |= (1UL << (bit)))
#define BIT_CLEAR(value, bit) ((value) &= ~(1UL << (bit)))
#define BIT_WRITE(value, bit, bitvalue) (bitvalue ? BIT_SET(value, bit) : BIT_CLEAR(value, bit))
#define OPEN LOW
#define CLOSED HIGH

const uint8_t MAXKEYS = 2;
uint32_t keyindex = 0;

//      keydata[MAXKEYS][KEYCODE,KEYSTATE,KEYCHANGE]
uint8_t keydata[MAXKEYS][3] = {{0xFF,0x00,0x00},{0xFF,0x00,0x00}};
uint8_t keybitmap[4];  // four rows by eight columns key bit map
static uint16_t holdTime = 500;
static uint8_t debounceTime = 12;

inline void nextkeystate(void) __attribute__((always_inline));

uint8_t findinlist(uint8_t kcode) 
{/* TODO: unroll function, redundant now with only 2 keys */

    for (uint8_t i=0; i<MAXKEYS; i++) 
    {
        if (keydata[i][KEYCODE] == kcode) {return i;}
    }
    return NOKEY;   
} 

void nextkeystate(uint8_t idx, bool button) 
{/* keys state machine and debouncer 
  * TODO - make double press
  */
  
    static uint32_t holdTimer = 0;
    keydata[idx][KEYCHANGE] = false;
    switch (keydata[idx][KEYSTATE]) 
    {
        case IDLE:
            if ( button==CLOSED ) 
            {
                keydata[idx][KEYSTATE] = PRESSED;
                holdTimer = millis(); 
            }
            break;
        case PRESSED:
            if ( (millis()-holdTimer)>uint32_t(holdTime) )
            {
                keydata[idx][KEYSTATE] = HOLD;
            } 
            else if (button==OPEN)
            {
                keydata[idx][KEYSTATE] = RELEASED;
            }
            break;
        case HOLD:
            if ( button==OPEN )
            {
                keydata[idx][KEYSTATE] = RELEASED;
            }
            break;
        case RELEASED:
            keydata[idx][KEYSTATE] = IDLE;
            break;
        default: break;
    }
    keydata[idx][KEYCHANGE] = true;
}

bool updatelist()
{/* Fills keydata[MAXKEYS][KEYCODE,KEYSTATE,KEYCHANGE] array with 2 active keys
  * Returns true if any keys on the list changed state 
  * TODO: Continue to unroll loops
  */
    bool anyactivity = false;
    // Delete any IDLE keys
    if ( keydata[0][KEYSTATE] == IDLE ) 
    {
        keydata[0][KEYCODE] = NOKEY;
        keydata[0][KEYCHANGE] = false;
    }       
    if ( keydata[1][KEYSTATE] == IDLE ) 
    {
        keydata[1][KEYCODE] = NOKEY;
        keydata[1][KEYCHANGE] = false;
    }       
    
    // Add new keys to empty slots in the key list.
    for ( uint8_t r=0; r<4; r++ ) 
    {
        for ( uint8_t c=0; c<8; c++ ) 
        {
            bool button = BIT_READ(keybitmap[r],c);
            uint8_t keyCode = NOKEY;
            keyCode = r * 8 + c;

            uint8_t idx = NOKEY;
            keydata[0][KEYCODE] == keyCode ? idx = 0x00 : idx = NOKEY;
            keydata[1][KEYCODE] == keyCode ? idx = 0x01 : idx = NOKEY;

            // key is already on the list so set its next state
            if ( idx < NOKEY )  {nextkeystate(idx, button);}

/*
            // key is not on the list so add it
            if ( (idx == NOKEY) && button ) {
                // for (uint8_t i=0; i<MAXKEYS; i++) {
                if ( keydata[0][KEYCODE] == NOKEY ) { // find first empty slot or skip if full
                    keydata[0][KEYCODE] = keyCode;
                    keydata[0][KEYSTATE] = IDLE;    
                    nextkeystate (0, button);// this will start debounce timer
                    //break;    // Don't fill all the empty slots with the same key.
                } else if ( keydata[1][KEYCODE] == NOKEY ) { // find first empty slot or skip if full
                    keydata[1][KEYCODE] = keyCode;
                    keydata[1][KEYSTATE] = IDLE;    
                    nextkeystate (1, button);// this will start debounce timer
                    //break;    // Don't fill all the empty slots with the same key.
                }
            } 
*/
            /* key is not on the list so add it */
            if ( (idx == NOKEY) && button ) 
            {
                for ( uint8_t i=0; i<MAXKEYS; i++ ) 
                {
                    if ( keydata[i][KEYCODE] == NOKEY ) 
                    { // find first empty slot or skip if full
                        keydata[i][KEYCODE] = keyCode;
                        keydata[i][KEYSTATE] = IDLE;    
                        nextkeystate (i, button);// start debounce timer
                        break;  // don't fill empty slots with the same key
                    }
                }
            }
        }
    }
    /* Report changed state of any key */
    if ( keydata[0][KEYCHANGE] ) anyactivity = true;
    if ( keydata[1][KEYCHANGE] ) anyactivity = true;

    return anyactivity;
}

bool updatelistOLD() 
{/* Fills keydata[MAXKEYS][KEYCODE,KEYSTATE,KEYCHANGE] array with 2 active keys
  * Returns true if any keys on the list changed state 
  */
    bool anyactivity = false;
    // Delete any IDLE keys
    for (uint8_t i=0; i<MAXKEYS; i++) 
    {
        if ( keydata[i][KEYSTATE] == IDLE ) 
        {
            keydata[i][KEYCODE] = NOKEY;
            keydata[i][KEYCHANGE] = false;
        }       
    }
    // Add new keys to empty slots in the key list.
    for (uint8_t r=0; r<4; r++) 
    {
        for (uint8_t c=0; c<8; c++) 
        {
            bool button = BIT_READ(keybitmap[r],c);
            uint8_t keyCode = NOKEY;
            //     programmer keys 0-15: selector pad 16-31
            c<=3 ? keyCode = r * 4 + c : keyCode = (r * 4) + (c + 12);
            uint8_t idx = findinlist(keyCode);
            // key is already on the list so set its next state
            if ( idx < NOKEY )  {nextkeystate(idx, button);}
            // key is not on the list so add it
            if ( (idx == NOKEY) && button ) 
            {
                for ( uint8_t i=0; i<MAXKEYS; i++ ) 
                {
                    if ( keydata[i][KEYCODE] == NOKEY ) 
                    {   // find first empty slot or skip if full
                        keydata[i][KEYCODE] = keyCode;
                        keydata[i][KEYSTATE] = IDLE;    
                        nextkeystate (i, button);// this will start debounce timer
                        break;  // Don't fill all empty slots with the same key.
                    }
                }
            }
        }
    }
    // Report changed state of any key
    //keysactive = 0;
    for ( uint8_t i=0; i<MAXKEYS; i++ ) 
    {
        //if ( keydata[i][KEYCODE] < NOKEY ) { keysactive++; }
        if ( keydata[i][KEYCHANGE] ) anyactivity = true;
    }
    return anyactivity;
}

void scankeys() 
{/* Read hardware port. Not using nested loops and bit shifts
  * key scanner now runs 5 times faster, approx 28µS.
  */
    DDRL = 0x01;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x00, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x00, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x00, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x00, !BIT_READ(PINB, 7));
    DDRL = 0x02;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x01, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x01, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x01, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x01, !BIT_READ(PINB, 7));
    DDRL = 0x04;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x02, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x02, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x02, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x02, !BIT_READ(PINB, 7));
    DDRL = 0x08;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x03, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x03, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x03, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x03, !BIT_READ(PINB, 7));
    DDRL = 0x10;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x04, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x04, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x04, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x04, !BIT_READ(PINB, 7));
    DDRL = 0x20;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x05, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x05, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x05, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x05, !BIT_READ(PINB, 7));
    DDRL = 0x40;
    delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x06, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x06, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x06, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x06, !BIT_READ(PINB, 7));
    DDRL = 0x80;
    cli(); // ensure uint32_t keyindex are not messed up by interrupt
    //delayMicroseconds(2);
    BIT_WRITE(keybitmap[0], 0x07, !BIT_READ(PINB, 4));
    BIT_WRITE(keybitmap[1], 0x07, !BIT_READ(PINB, 5));
    BIT_WRITE(keybitmap[2], 0x07, !BIT_READ(PINB, 6));
    BIT_WRITE(keybitmap[3], 0x07, !BIT_READ(PINB, 7));

    // create unique index for fast key(s) validation
    keyindex = keybitmap[3];
    keyindex <<= 8;
    keyindex |= keybitmap[2];
    keyindex <<= 8;
    keyindex |= keybitmap[1];
    keyindex <<= 8;
    keyindex |= keybitmap[0];
    
    sei();
}

bool getkeys() 
{/* Limit how often the keypad is scanned  */

    static uint32_t lastcall = 0;
    bool keyActivity = false;
    if ( (millis()-lastcall) > debounceTime ) 
    {
        scankeys();
        keyActivity = updatelist();
        lastcall = millis();
    }
    return keyActivity; 
}

void dokeys() 
{/* Call keypad scan and respond to key actions */

    //startA = micros();    
    if ( getkeys() ) 
    {
      for (int i=0; i<MAXKEYS; i++) 
      {
        if ( keydata[i][KEYCHANGE] ) 
        { 
          switch (keydata[i][KEYSTATE]) 
          {
            case IDLE:    /****************************************************/
                /* 
                Serial.print(keyindex);Serial.print(" - Key ");
                Serial.print(keydata[i][KEYCODE]);Serial.print(" - ");
                Serial.println(" IDLE");
                break; 
                */
            case PRESSED: /****************************************************/
                /* ----- ALTER MODE ----- */
                switch ( keyindex ) 
                { 
                  case 0x8000000: // sw16
                    switch ( flagmode ) 
                    {
                        case modKit:
                            flagmode = modChannel;
                            break;
                        case modChannel:
                            flagmode = modProg;
                            break;
                        case modProg:
                            flagmode = modKit;
                            break;
                        default: break;                     
                    }
                    break;
                  case 0x80000000: // selpad16
                    switch ( flagmode ) 
                    {
                        case modKit:
                            // change to kit 16 //flagmode = modChannel;
                            break;
                        case modChannel:
                            flagmode = modProg;
                            break;
                        case modProg:
                            flagmode = modKit;
                            break;
                        default: break;                     
                    }
                    default: break; 
                } 
                /* ----- END ALTER MODE ----- */

                /* ----- ALTER EDIT MODE ----- */
                if ( flagmode == modProg ) 
                { 
                    switch ( keyindex ) 
                    {
                      case 0x01: case 0x10:            // sw1 - selpad1
                        editMode = editAnPitch;                     
                        break;
                      case 0x02: case 0x20:            // sw2 - selpad2
                        editMode = editAnBend;
                        break;
                      case 0x04: case 0x40:            // sw3 - selpad3
                        editMode = editAnMod;
                        break;
                      case 0x08: case 0x80:            // sw4 - selpad4
                        editMode = editAnLevel;
                        break;
                      case 0x100: case 0x1000:         // sw5 - selpad5
                        editMode = editDigPitch;
                        break;
                      case 0x200: case 0x2000:         // sw6 - selpad6
                        editMode = editDigBend;
                        break;
                      case 0x400: case 0x4000:         // sw7 - selpad7
                        editMode = editDigMod;
                        break;
                      case 0x800: case 0x8000:         // sw8 - selpad8
                        editMode = editDigLevel;
                        break;
                      case 0x10000: case 0x100000:     // sw9 - selpad9
                        editMode = editFiltFreq;
                        break;
                      case 0x20000: case 0x200000:     // sw10 - selpad10
                        editMode = editFiltBend;
                        break;
                      case 0x40000: case 0x400000:     // sw11 - selpad11
                        editMode = editFiltRes;
                        break;
                      case 0x80000: case 0x800000:     // sw12 - selpad12
                        editMode = editFiltDecay;
                        break;
                      case 0x1000000: case 0x10000000: // sw13 - selpad13
                        editMode = editClick;
                        break;
                      case 0x2000000: case 0x20000000: // sw14 - selpad14
                        editMode = editNoise;
                        break;
                      case 0x4000000: case 0x40000000: // sw15 - selpad15
                        editMode = editModSpeed;
                        break;
                      default: break;
                    }
                    wheelposition = kitdata[0][(activechannel*editMode)-1];
					#if DEBUG >= 1
						Serial.print("wheelposition write: ");
						Serial.println(wheelposition);
					#endif
                }
                
if (keyindex > 0)
{
Serial.print("keyindex: ");Serial.println(keyindex);
Serial.print("flagmode: ");Serial.println(flagmode);
Serial.print("editMode: ");Serial.println(editMode);
}
//Serial.print(keybitmap[3],BIN);Serial.print(keybitmap[2],BIN);Serial.print(keybitmap[1],BIN);Serial.println(keybitmap[0],BIN);
//Serial.print(keyindex);Serial.print(" - Key ");Serial.print(keydata[i][KEYCODE]);Serial.print(" - ");Serial.println(" PRESSED");
                break;
            case HOLD:     /*************************************************************/

                Serial.print("0x");Serial.println(keyindex,HEX);
                Serial.print(keybitmap[3],BIN);Serial.print("|");Serial.print(keybitmap[2],BIN);Serial.print("|");Serial.print(keybitmap[1],BIN);Serial.print("|");Serial.println(keybitmap[0],BIN);
                Serial.print(keyindex,BIN);Serial.print(" - Key ");Serial.print(keydata[i][KEYCODE]);Serial.print(" - ");Serial.println(" HOLD");
                break;
            case RELEASED: /*************************************************************/
                
//Serial.print(keyindex);Serial.print(" - Key ");Serial.print(keydata[i][KEYCODE]);Serial.print(" - ");Serial.println(" RELEASED");
                break;
            
            default: break;
            }
        }
      }
    }
//stopA = micros(); Serial.print(stopA-startA);Serial.println(" micro seconds total time for dokeys()");
}

void initkeypad() 
{/* Startup only
  *  PB7-PB4 row output pins for 4x8 switch matrix 
  *  PL3-PL0 col return pins for programmer pcb
  *  PL7-PL4 col return pins for selector pad   
  */
    PORTB |= 0xF0; // pull-ups ON  0b11110000
    DDRB |= 0x00;  // outputs      0b00000000
    DDRL = 0x00;   // outputs when idle
    PORTL = 0x00;  // pull-ups OFF
}

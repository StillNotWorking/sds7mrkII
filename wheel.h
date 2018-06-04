/* Simmons SDS7 mrk-II w/m2560 
 * Encoder Wheel
 * program logic from https://github.com/PaulStoffregen/Encoder
 */

//#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
//#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
//#define DIRECT_PIN_READ(base, mask)     (((*(base)) & (mask)) ? 1 : 0)

const uint8_t pin1 = 0x12; // 18
const uint8_t pin2 = 0x13; // 19
//static uint8_t pin1_register = PIN_TO_BASEREG(pin1);
//static uint8_t pin1_bitmask = PIN_TO_BITMASK(pin1);
//static uint8_t pin2_register = PIN_TO_BASEREG(pin2);
//static uint8_t pin2_bitmask = PIN_TO_BITMASK(pin2);
volatile uint8_t wheelposition = 0;
volatile uint8_t wheelstate = 0;

void encoderISR() 
{/* Encoder Interrupt Service Routine
  * Connects to PD2 and PD3 (pin 18, 19) Center tap to +5V
  */
	uint8_t p1val = PIND & (1<<2) ? 0:1;
	uint8_t p2val = PIND & (1<<3) ? 0:1;
	uint8_t state = wheelstate & 3;
	if (p1val) { state |= 4; }
	if (p2val) { state |= 8; }
	wheelstate = (state >> 2);
	switch (state) 
	{
		case 1: case 7: case 8: case 14:
			//if ( wheelposition > 0xFF ) { wheelposition++; }
			wheelposition++;
			return;
		case 2: case 4: case 11: case 13:
			//if ( wheelposition < 0x00 ) { wheelposition--; }
			wheelposition--;
			return;
		case 3: case 12:
			//if ( wheelposition > 0xFD ) { wheelposition += 2; }
			wheelposition += 2;
			return;
		case 6: case 9:
			//if ( wheelposition < 0x02 ) { wheelposition -= 2; }
			wheelposition -= 2;
			return;
	}
}

void dowheel() 
{/* Read encoder buffer and respond to new values */

    static int32_t wheelprevread  = 0x00;

    if ( wheelposition != wheelprevread ) 
    {
		wheelprevread = wheelposition;
        if ( wheelposition > 0xFF ) { wheelposition = 0xFF; }
        if ( wheelposition < 0x00 ) { wheelposition = 0x00; }
    
		switch (flagmode) 
		{
		  case modKit:
			//change kit
			break;  
		  case modProg:
			kitdata[0][(editMode - 1) + activechannel] = wheelposition;
			break;
		  case modMIDI:
			//edit MIDI
			break;
		  default: break;
		}
		#if DEBUG >= 1
			Serial.println(wheelposition);
		#endif
	}
}

void initwheel() 
{/* Startup only 
  * Encoder connects to PD2 and PD3 (pin 18, 19)
  */
	/*PORTD |= 0x0C; // pin 2,3 pull-ups ON
    DDRD  |= 0x0C; // pin 2,3 Inputs */
	PORTD &= ~0x0C; // pin 2,3 pull-ups OFF
    DDRD  |=  0x0C; // pin 2,3 Inputs
	/*PORTD &= ~(1 << 2);
	PORTD &= ~(1 << 3);
	DDRD &= ~(1 << 2);
	DDRD &= ~(1 << 3);*/

	attachInterrupt(digitalPinToInterrupt(pin1), encoderISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(pin2), encoderISR, CHANGE);
	
	uint8_t s = 0;
	if (PIND & (1<<2) ? 0:1) { s |= 1; }
	if (PIND & (1<<3) ? 0:1) { s |= 2; }
	wheelstate = s;
}
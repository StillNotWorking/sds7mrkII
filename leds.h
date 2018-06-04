/* Simmons SDS7 mrk-II w/m2560
 * LED control 
 */

/* 7 segment cathodes on port K
 * 0123456789ACcdEF
 */
const PROGMEM uint8_t digits[] = 
{
	0b00111111, 
	0b00000110, 
	0b01011011, 
	0b01001111,
	0b01100110,
	0b01101101, 
	0b01111101,
	0b00000111, 
	0b01111111, 
	0b01100111,
	0b01110111,
	0b00111001,
	0b01011000,
	0b01011110,
	0b01111001,
	0b01110001
};//  -gfedcba
      
void amux() 
{
/* multiplex anode pins to keep total current down
 * Port Cp7-0 7-seg anode and matrix r1-3
 * Port Dp7   matrix r4
 * Port Fp7,6 VC - test without voice card to keep noise down.
 */
    static uint8_t muxpos = 0x00;
	muxpos++;
	switch ( muxpos ) 
	{
		case 0x01:
			DDRD &= ~0x7F; // mux_r4 anode off
			DDRC = 0xFE; // 7seg anode 
			break;
		case 0x02:
			DDRC = 0xFD; // 7seg anode 
			break;
		case 0x03:
			DDRC = 0xFB; // 7seg anode 
			break;
		case 0x04:
			DDRC = 0xF7; // 7seg anode 
			break;
		case 0x05:
			DDRC = 0xEF; // 7seg anode 
			break;
		case 0x06:
			DDRC = 0xDF; // mux_r1 anode 
			break;
		case 0x07:
			DDRC = 0xBF; // mux_r2 anode 
			break;
		case 0x08:
			DDRC = 0x7F; // mux_r3 anode 
			break;
		case 0x09:
			DDRC = 0xFF;
			DDRD |= 0x7F; // mux_r4 anode on
			break;
		default: break;
	}
}

void cmux() 
{
	// multiplex cathode pins on 7-segment to keep total current down
	// execute when amux() activate anode for 7-segment on port Cp4-0
	// Port Kp6-0
	
	// for safety this check can be done, but slows down the multiplexer
	//if ( PORTC == 16 || PORTC == 8 || PORTC == 4 || PORTC == 2 || PORTC == 1) {}
}

void dosegments() 
{

}

void errordisplay(uint8_t errorcode) 
{
    switch (errorcode) 
	{
        case errsdcard:
          break;
    }
}

void vcled(uint8_t avc) 
{/* Light up Prog LED on a single voice card to indicate which card being edited
  * active voice card avc = 1-12, else all LED's off
  * Port Fp0-5 Cathodes vc1-7,2-8,3-9,4-10,5-11,6-12
  * Port Fp6 Anode vc1-6 - Port Fp7 Anode vc7-12  
  */  
    uint8_t cmask = 0x3F; //DDRF  0b00111111 0=output 1=input;
    uint8_t amask = 0x0;  //PORTF 0b00000000

    if( avc >=1 && avc <=6 ) 
	{
      cmask = 0x01 << ( avc - 1 ); //0b00000001
      amask = 0x20;                //0b00100000
    }
    if( avc >=7 && avc <=12 ) 
	{
      cmask = 0x01 << ( avc - 7 ); //0b00000001
      amask = 0x40;                //0b01000000
    }  
    
    DDRF = 0x3F ^ cmask; // 0b00111111 0=output 1=input;
    PORTF = amask;

    #if DEBUG == 1
      Serial.print("Active Voice Card: "); Serial.println(avc);
      Serial.print("cmask: "); Serial.println(b2s(cmask));
      Serial.print("amask: "); Serial.println(b2s(amask));    
      Serial.print("PINF:  "); Serial.println(b2s(PINF));
      Serial.print("PORTF: "); Serial.println(b2s(PORTF));
      Serial.print("DDRF:  "); Serial.println(b2s(DDRF));    
    #endif
}

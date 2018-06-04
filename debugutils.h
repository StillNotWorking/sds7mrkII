char* b2s(uint8_t var) {
   static char str[8];
   uint8_t mask = 0b10000000;
   for(int i = 0; i<8; i++){
     str[i]='0';
     if(((mask >> i) & var) == (mask>>i)){
        str[i]='1';
     }
   }
   str[8] = '\0';
   return str;
}
/*  SD card reference: https://www.arduino.cc/en/Reference/SD
 *   
 *  Timer Interrupts: 
 *  page 145 m2560 datasheet explains CTC mode - Clear Timer on Compare Match
 *  page 126 Register Description with charts 
 *  page 157 Prescale Table
 *  useful explanation http://www.instructables.com/id/Arduino-Timer-Interrupts/
 *  iomxx0_1.h has all the defines for timer setup
 *  #define TIMSK5  _SFR_MEM8(0x73)
 *  #define ICIE5   5
 *  #define OCIE5C  3
 *  #define OCIE5B  2
 *  #define OCIE5A  1
 *  #define TOIE5   0
 *  #define TCCR5A  _SFR_MEM8(0x120)
 *  #define COM5A1  7
 *  #define COM5A0  6
 *  #define COM5B1  5
 *  #define COM5B0  4
 *  #define COM5C1  3
 *  #define COM5C0  2
 *  #define WGM51   1
 *  #define WGM50   0
 *  #define TCCR5B  _SFR_MEM8(0x121)
 *  #define ICNC5   7
 *  #define ICES5   6
 *  #define WGM53   4
 *  #define WGM52   3
 *  #define CS52    2
 *  #define CS51    1
 *  #define CS50    0
 *  Set prescaler bits: 
 *  TCCR5B |= (1 << CS52) | (1 << CS51) | (1 << CS50);
 *  CSn2 CSn1 CSn0 Description
 *   0    0    0   No clock source. (Timer/Counter stopped)
 *   0    0    1   clkI/O/1 (No prescaling
 *   0    1    0   clkI/O/8 (From prescaler)
 *   0    1    1   clkI/O/64 (From prescaler)
 *   1    0    0   clkI/O/256 (From prescaler)
 *   1    0    1   clkI/O/1024 (From prescaler)
 *   1    1    0   External clock source on Tn pin. Clock on falling edge
 *   1    1    1   External clock source on Tn pin. Clock on rising edge
 *   Set match register to 1Hz
 *  OCR5A = 0x3D08; //   1Hz  = 15624 = (16*10^6) / (1*1024) - 1 (max 65536)
 *  
    Adafruit MCP4725
    https://github.com/adafruit/Adafruit_MCP4725
     *  For Adafruit MCP4725A1 the address is 0x62 (default) 
     *  or 0x63 (ADDR pin tied to VCC)
     *  For MCP4725A0 the address is 0x60 or 0x61
     *  For MCP4725A2 the address is 0x64 or 0x65 
    @param[in]  output
                The 12-bit value representing the relationship between
                the DAC's input voltage and its output voltage.
    @param[in]  writeEEPROM
                If this value is true, 'output' will also be written
                to the MCP4725's internal non-volatile memory, meaning
                that the DAC will retain the current voltage output
                after power-down or reset.
    One thing thats a little annoying about the Arduino Wire library 
    in this case is it is set for 100KHz transfer speed. In the MCP4725 
    library we update the speed to 400KHz by setting the TWBR
        TWBR = 12; // 400 khz
        

---------------------------------------------------------------------------
ENCODER:  https://github.com/PaulStoffregen/Encoder		
//                           _______         _______       
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

		//	new	new	old	old
		//	pin2	pin1	pin2	pin1	Result
		//	----	----	----	----	------
		//	0	0	0	0	no movement
		//	0	0	0	1	+1
		//	0	0	1	0	-1
		//	0	0	1	1	+2  (assume pin1 edges only)
		//	0	1	0	0	-1
		//	0	1	0	1	no movement
		//	0	1	1	0	-2  (assume pin1 edges only)
		//	0	1	1	1	+1
		//	1	0	0	0	+1
		//	1	0	0	1	-2  (assume pin1 edges only)
		//	1	0	1	0	no movement
		//	1	0	1	1	-1
		//	1	1	0	0	+2  (assume pin1 edges only)
		//	1	1	0	1	-1
		//	1	1	1	0	+1
		//	1	1	1	1	no movement		
		
*/
                 

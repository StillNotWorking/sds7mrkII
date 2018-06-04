/*******************************************************************************
* Simmons SDS7 mark II with Mega2560 to replace original CPU and Memory PCB's
*
* initial planning files 2018.03.10
* project page http://snw.lonningdal.no/sds7mc.php
*
* Preset memory are structured as complete kits with  
* 16 parameters by 12 voice card — A Preset/program 
* equals a Kit with total size of 192 bytes
*
* Library Edits: 
*    ...\Arduino15\packages\arduino\hardware\avr\1.6.21\cores\arduino\Arduino.h
*    unsigned long millis(void) __attribute__((always_inline));
*
*******************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <MIDI.h>
#include <EEPROM.h>

#define DAC_RESOLUTION (8)
#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;

/* This optional setting causes Encoder to use more optimized code,
 * It must be defined before Encoder.h is included.              
// #define ENCODER_OPTIMIZE_INTERRUPTS
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
Encoder wheel(0x12, 0x13); // pin 18, 19  */

/**************************************************\
 * REMEMEBR TO SET DEBUG TO 0 FOR PRODUCTION CODE *
\**************************************************/
#define DEBUG 3
#if DEBUG >= 1
    unsigned long loopCount;
    unsigned long startTime;
#endif


//    undefine unsigned long millis(void); 
//  unsigned long millis(void) __attribute__((always_inline));


// SD default chip select pin.
const uint8_t chipSelect = SS;
File root;

#include "constants.h"
// flagmode (Control Modus) tell which mode the control panel are in
volatile uint8_t flagmode = modKit;
//volatile uint8_t flagpreviousmode = modKit;
volatile bool flagloadkits = false;
uint8_t activechannel = 0x00;
uint8_t editMode = editOFF;
volatile bool flagcv = false; // trigger CV function from timer

/* kitdata[0] are current selected kit, rest will be loaded with nearby kits */
volatile uint8_t kitdata[5][192] = 
{
{
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x01,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x02,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x03,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x04,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x05,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x06,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x07,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x08,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x09,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x0A,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x0B,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x0C
},
{
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x0D,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x0E,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x0F,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x10,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x11,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x12,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x13,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x14,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x15,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x16,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x17,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x18
},
{
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x19,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x1A,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x1B,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x1C,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x1D,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x1E,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x1F,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x20,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x21,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x22,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x23,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x24
},
{
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x25,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x26,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x27,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x28,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x29,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x2A,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x2B,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x2C,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x2D,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x2E,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x2F,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x30
},
{
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x31,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x32,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x33,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x34,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x35,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x36,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x37,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x38,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x39,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x4A,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x4B,
0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x4C
}
};
#include "memory.h"
#include "debugutils.h"
#include "wheel.h"
#include "switches.h"
#include "midi.h"
#include "leds.h"
#include "cvcontrol.h"


MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

uint8_t midich = MIDI_CHANNEL_OMNI;

// hold kit-numbers for selector pad
uint8_t selpad[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                      0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F}; 
// midi note associated with with voice card, C1-C2
uint8_t vcnote[12] = {0x24,0x25,0x26,0x27,0x28,0x29,
                      0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};
// temporary buffer for a single voice card's 16 parameters
volatile uint8_t vcbuffer[16] = { 0x80,0x80,0x00,0xFF,0x80,0x80,0x00,0xFF,
                                  0xFF,0x80,0x00,0xFF,0x00,0x00,0x00,0x00 };

ISR(TIMER5_COMPA_vect)
{/* approx 7.3kHz */
    flagcv = true;
}

void setup() 
{
    // DDR 0=output 1=input
    // PA: MSB=Parameter LSB=Voice card     
    DDRA  = 0x00;  //              0b00000000
    PORTA = 0x00;  //              0b00000000   
    // PF: Voice Card Prog LED
    DDRF  = 0x3F;  //              0b00111111
    PORTF = 0x00;  //              0b00000000
    initkeypad();  // set up ports for keypad
	initwheel();
	
    // set timer for control voltage update speed
    cli();      // stop all interrupts        
        TCCR5A = 0; TCCR5B = 0; TCNT5  = 0; // clear registers and counter
        // compare match register = (16,000,000Hz/(prescaler * desired interrupt frequency)) - 1
        OCR5A = 0x88E; // 7.3kHz =  2190 = (16000000 / 7300) -1   (no prescaler)       
        TCCR5B |= (1 << WGM52);  // turn on CTC mode
        TCCR5B |= (1 << CS50);   // set bit for prescaler off      
        TIMSK5 |= (1 << OCIE5A); // enable timer compare interrupt
    sei();      // enable interrupts    
          
    #if DEBUG >= 1
      while(!Serial);
      //Serial.begin(9600);
      Serial.begin(115200);
      Serial.println("Arduino ready for SDS7"); Serial.println("");
      Serial.println("Initializing SD card...");      
    #endif

    if (!SD.begin()) 
    {
      errordisplay(errsdcard);   
      #if DEBUG >= 1
          Serial.println("SD card initialization failed!"); 
      #endif  
    }
    dac.begin(0x62);
        
    MIDI.setHandleNoteOn(handleNoteOn);  
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleProgramChange(handleProgramChange);
    MIDI.setHandleSystemExclusive(handleSysEx);
    MIDI.setHandlePitchBend(handlePitchBend);
    MIDI.begin(midich);                

    //k2r(startupkit());
    //loadselpad();

    /* Light up prog LED on each voice card one by one */
    for(int8_t i=12; i>-1; i--)
    {
        vcled(i);
        delay(100); // this should be used to visualize other aspects at boot
    }

    #if DEBUG == 1
        // simulate timer events to CV
        for(uint8_t i=0; i<192; i++) { cv(); }
        // force jump 
        Serial.println("Force jump in CV (control voltage) routine");
        cv(0x06,editFiltFreq);
        Serial.println("Back to normal in CV distribution");
        cv();
    #endif  
}

void loop() 
{
    #if DEBUG == 3
        loopCount++;
        if ( (millis()-startTime)>5000 ) 
        {
            Serial.print("Average loops per second = "); 
            Serial.println(loopCount/5);
            startTime = millis();
            loopCount = 0;
        }
    #endif
    if (flagcv) { cv(); };
    MIDI.read();
    if (flagcv) { cv(); };
    dokeys();
    if (flagcv) { cv(); };
    dowheel();
}
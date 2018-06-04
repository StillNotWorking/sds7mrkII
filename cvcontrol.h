/* Simmons SDS7 mrk-II w/m2560
 * Control Voltage for voice cards  
 *  
 */

extern long count;
 
void cv(uint8_t vc = 0, uint8_t param = 0) {
/* send control voltage to voice cards, called from timer event or flagcv
 * Port A, MSB:VoiceCard, LSB:Parameter 
 */
    static uint8_t vccount = 1;
    static uint8_t pcount = 0;
    uint8_t datapos;
    //cli(); // disable interrupts if called directly from timer event
    
    // priority switch to ensure fast execution when 
    // voice parameters change with midi event
    if ( vc > 0 ) {
        PORTA = (vc << 4) | param;
        datapos = (param + (16*(vc-1)));
    } else {
        PORTA = (vccount << 4) | pcount;
        datapos = (pcount + (16*(vccount-1)));  
    }
    
    // voltage should be scaled 255 = max 2.5V
    dac.setVoltage(kitdata[0][datapos], false);
    
    #if DEBUG == 1
        if ( vc > 0 ) {
          Serial.print("Voice Card: "); Serial.print(vc);
          Serial.print(" - Parameter: "); Serial.println(param);          
        } else {
          Serial.print("Voice Card: "); Serial.print(vccount);
          Serial.print(" - Parameter: "); Serial.println(pcount);
        }
        Serial.print("kitdata: "); Serial.print(kitdata[0][datapos]);        
        Serial.print("  -  PORTA: 0b"); Serial.println(PORTA,BIN);
    #endif

    // prepare for next timer event
    if ( pcount >= 15 ) {
        pcount = 0;
        vccount++;
    } else {
        pcount++;  
    }
    if ( vccount>=13 ) {vccount = 1;}
    flagcv = false; 
    // sei(); // enable interrupts 
}

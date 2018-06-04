// Simmons SDS7 mark II with Mega2560
// -----------------------------------------------------------------------------
// MIDI Library manual: http://arduinomidilib.fortyseveneffects.com/index.html
// Keep MIDI callbacks short 
// -----------------------------------------------------------------------------

void handleProgramChange(uint8_t channel, uint8_t kitno){
  
}
void handleSysEx(uint8_t *array, unsigned arsize){
  
}
void handlePitchBend(uint8_t channel, int bend){
  
}
void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity){
    // we do not handle triggers but can alter sounds
    // kit preset switching for octaves has been discussed
}
void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity){

}

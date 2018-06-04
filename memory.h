/* Simmons SDS7 mrk-II w/m2560 
 * Memory
 */
 
void k2r(uint8_t kit) {
  // load kit(preset) to RAM
}
void copyvcparameter(uint8_t vcnumber) {
  // read into buffer memory all 16 parameters for the selected voice card in active Kit
  //vcbuffer[] = ;
}
void writevc2ram(uint8_t vcnumber) {
  // write to RAM from buffer, to selected voice card in current active Kit
}
void savekit(uint8_t memlocation) {
  // save active kit to flash
  // maybe not different memory type but automatically select SD card from a given preset number
}
void doautosave() {
  // if AUTOSAVE==true save any changes to associated kit,  
  // make sure not to overwrite when preset are changed
  // take into account flash wear and maybe use time trigger
}
void saveselpad(uint8_t swnumber) {
  // save current active kit number with kit selector pad switch number
  //selpad[swnumber] = ;
}
void loadselpad() {
  // load kit-map for selector pad, might be dynamic if/when dynamic catalog change on SD card
  //selpad[] = ;
}
void loadkit(uint8_t kitnumber){
  // change active kit
}
uint8_t startupkit() {
  // read last used kit number from flash
  return true;
}
void savestartupkit() {
  // save current kit number to flash for next startup
}
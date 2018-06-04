//Simmons SDS7 mrk-II w/m2560
//Constants 

const uint8_t NOKEY = 0xFF;
const uint8_t IDLE = 0x00;
const uint8_t PRESSED = 0x01;
const uint8_t HOLD = 0x02;
const uint8_t RELEASED = 0x03;
const uint8_t KEYCODE = 0x00;
const uint8_t KEYSTATE = 0x01;
const uint8_t KEYCHANGE = 0x02;
const uint8_t editOFF = 0x00;
const uint8_t editAnPitch = 0x01;
const uint8_t editAnBend = 0x02;
const uint8_t editAnMod = 0x03;
const uint8_t editAnLevel = 0x04;
const uint8_t editDigPitch = 0x05;
const uint8_t editDigBend = 0x06;
const uint8_t editDigMod = 0x07;
const uint8_t editDigLevel = 0x08;
const uint8_t editFiltFreq = 0x09;
const uint8_t editFiltBend = 0x0A;
const uint8_t editFiltRes = 0x0B;
const uint8_t editFiltDecay = 0x0C;
const uint8_t editClick = 0x0D;
const uint8_t editNoise = 0x0E;
const uint8_t editModSpeed = 0x0F;
const uint8_t modKit = 0xA0;
const uint8_t modProg = 0xA1;
const uint8_t modChannel = 0xA2;
const uint8_t modMIDI = 0xA3;
const uint8_t modSD = 0xA4;
const uint8_t modSettings = 0xA5;
const uint8_t errsdcard = 0xE1;

#define CLK1 3
#define CLK2 2
#define CLK3 1
#define DOUT1 11
#define DOUT2 10
#define DOUT3 9

boolean is_ready = false; 
  
void setup() {
  pinMode(CLK1, OUTPUT); 
  pinMode(CLK2, OUTPUT); 
  pinMode(CLK3, OUTPUT); 
  pinMode(DOUT1, INPUT);
  pinMode(DOUT2, INPUT);
  pinMode(DOUT3, INPUT); 
  Serial.begin(115200);
}

void loop() {
  Serial.println(HX711read(DOUT3,CLK3));
  delay(20);
  

}
//void HX711MULTI::readRaw(long *result) {
//  int i,j;
//  // wait for all the chips to become ready
//  while (!is_ready());
//
//  // pulse the clock pin 24 times to read the data
//  for (i = 0; i < 24; ++i) {
//    digitalWrite(PD_SCK, HIGH);
//    if (NULL!=result) {
//      for (j = 0; j < COUNT; ++j) {
//        bitWrite(result[j], 23-i, digitalRead(DOUT[j]));
//      }
//    }
//    digitalWrite(PD_SCK, LOW);
//  }
//   
//  // set the channel and the gain factor for the next reading using the clock pin
//  for (i = 0; i < GAIN; ++i) {
//    digitalWrite(PD_SCK, HIGH);
//    digitalWrite(PD_SCK, LOW);
//  }
//
//    // Datasheet indicates the value is returned as a two's complement value, so 'stretch' the 24th bit to fit into 32 bits. 
//    if (NULL!=result) {
//      for (j = 0; j < COUNT; ++j) {
//        if ( ( result[j] & 0x00800000 ) ) {
//          result[j] |= 0xFF000000;
//        } else {
//          result[j] &= 0x00FFFFFF; //required in lieu of re-setting the value to zero before shifting bits in.
//        }
//      } 
//
//    }
//}

//long EEPROMReadlong(long address) {
//  long four = EEPROM.read(address);
//  long three = EEPROM.read(address + 1);
//  long two = EEPROM.read(address + 2);
//  long one = EEPROM.read(address + 3);
// 
//  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
//}

//void EEPROMWritelong(int address, long value) {
//  byte four = (value & 0xFF);
//  byte three = ((value >> 8) & 0xFF);
//  byte two = ((value >> 16) & 0xFF);
//  byte one = ((value >> 24) & 0xFF);
// 
//  EEPROM.write(address, four);
//  EEPROM.write(address + 1, three);
//  EEPROM.write(address + 2, two);
//  EEPROM.write(address + 3, one);
//}

long prepocet (long pedalMin, long pedalMax, long hodnota){
  if((pedalMax - pedalMin)>65535){
    long delitel = (pedalMax-pedalMin)/65535 + 1;
    long offset = pedalMin/delitel + 32767;
    return (hodnota/delitel - offset);
  }
  else {
    long offset = pedalMin + 32767;
    return (hodnota-offset);
  }
  
}

float HX711read(int DOUT,int CLK) 
{
  // this waiting takes most time...
  while (digitalRead(DOUT) == HIGH) yield();
  
  union
  {
    long value = 0;
    uint8_t data[4];
  } v;

  noInterrupts();

  // Pulse the clock pin 24 times to read the data.
  v.data[2] = shiftIn(DOUT, CLK, MSBFIRST);
  v.data[1] = shiftIn(DOUT, CLK, MSBFIRST);
  v.data[0] = shiftIn(DOUT, CLK, MSBFIRST);

  // TABLE 3 page 4 datasheet
  // only default verified, so other values not supported yet
  uint8_t m = 1;   // default gain == 128
  int gain = 32;
  if (gain == 64) m = 3;
  if (gain == 32) m = 2;

  while (m > 0)
  {
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
    m--;
  }

  interrupts();

  // SIGN extend
  if (v.data[2] & 0x80) v.data[3] = 0xFF;

  //_lastRead = millis();
  return 1.0 * v.value;
}
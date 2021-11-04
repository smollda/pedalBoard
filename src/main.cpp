#include <EEPROM.h>
#include <Arduino.h>
#include "Joystick.h"
#define CLK1 3
#define CLK2 2
#define CLK3 0
#define DOUT1 11
#define DOUT2 10
#define DOUT3 9
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
JOYSTICK_TYPE_JOYSTICK, 11, 0,
true, true, true, true, true, true,
true, true, true, true, true);

bool is_ready = false; 
  
void setup() {
  Joystick.begin();
  pinMode(CLK1, OUTPUT); 
  pinMode(CLK2, OUTPUT); 
  pinMode(CLK3, OUTPUT); 
  pinMode(DOUT1, INPUT);
  pinMode(DOUT2, INPUT);
  pinMode(DOUT3, INPUT); 
  digitalWrite(CLK1, LOW);
  digitalWrite(CLK2, LOW);
  digitalWrite(CLK3, LOW);
  Serial.begin(115200);
  Joystick.setXAxisRange(-32767, 32767);
  Joystick.setYAxisRange(-32767, 32767);
  Joystick.setZAxisRange(-32767, 32767);
  Joystick.setBrakeRange(-32767, 32767);
}

long EEPROMReadlong(long address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
 
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWritelong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
 
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

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

long HX711read(int DOUT,int CLK) 
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
  //int gain = 32;
  //if (gain == 64) m = 3;
  //if (gain == 32) m = 2;

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
  return v.value;
}


void loop() {
  //Serial.print(HX711read(DOUT3,CLK3));
  //Serial.print("                 ");
  Serial.print(HX711read(DOUT2,CLK2));
  Serial.print("                 ");
  Serial.println(HX711read(DOUT1,CLK1));
  //Joystick.setXAxis(constrain(prepocet(440000,720000,HX711read(DOUT3,CLK3)),-32767,32767));  // Brzda
  delay(5);
  

}



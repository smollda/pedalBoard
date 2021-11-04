#include <EEPROM.h>
#include <Arduino.h>
#include "Joystick.h"
#define CLK1 3
#define CLK2 2
#define CLK3 0
#define DOUT1 11
#define DOUT2 10
#define DOUT3 9
#define JOYSTICK_DEFAULT_REPORT_ID 0x06
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
JOYSTICK_TYPE_JOYSTICK, 11, 0,
true, true, true, true, true, true,
true, true, true, true, true);
long pedal1min = 0;
long pedal1max = 0;
long pedal2min = 0;
long pedal2max = 0;
long pedal3min = 0;
long pedal3max = 0;
long ruckaMin = 0;
long ruckaMax = 0;
long pedal1 = 0;
long pedal2 = 0;
long pedal3 = 0;
long kalibraceZacatek;
bool kalibrace = false;
bool kalibraceStart = false;
char recievedChar;
bool is_ready = false; 
long cekaniZacatek = 0;
long cekani = 0;
  
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
  return v.value;
}

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
  Serial.println("Setup");
  if (EEPROMReadlong(0) == -1)
  {  
    kalibrace = true;
    kalibraceStart = true;
    kalibraceZacatek = 0;
  }
  else
  {
    pedal1min = EEPROMReadlong(0);
    pedal1max = EEPROMReadlong(4);
    pedal2min = EEPROMReadlong(8);
    pedal2max = EEPROMReadlong(12);
    pedal3min = EEPROMReadlong(16);
    pedal3max = EEPROMReadlong(20);
    ruckaMin = EEPROMReadlong(24);
    ruckaMax = EEPROMReadlong(28);
  }
  Joystick.setXAxisRange(-32767, 32767);
  Joystick.setYAxisRange(-32767, 32767);
  Joystick.setZAxisRange(-32767, 32767);
  Joystick.setBrakeRange(-32767, 32767);
}


void loop() {
  pedal1 = HX711read(DOUT1,CLK1);
  pedal2 = HX711read(DOUT2,CLK2);
  //pedal3 = HX711read(DOUT3,CLK1);
  //Serial.println(cekani);
  Joystick.setXAxis(constrain(prepocet(pedal1min,pedal1max,pedal1),-32767,32767));
  Joystick.setYAxis(constrain(prepocet(pedal2min,pedal2max,pedal2),-32767,32767));
  Joystick.setZAxis(constrain(prepocet(pedal3min,pedal3max,pedal3),-32767,32767));
  //Načtení znaku z konzole pro spuštění kalibrace
  if (Serial.available() > 0) {
    // read the incoming byte:
    recievedChar = Serial.read();

    // say what you got:
    if (recievedChar == 'k'){
      kalibrace = true;
      kalibraceStart = true;
      kalibraceZacatek = millis();
      Serial.println("Kalibrace:");
      recievedChar = " ";
    }
  }
  //Start kalibrace
  if (kalibraceStart)
  {
    pedal1min = pedal1;
    pedal2min = pedal2;
    pedal3min = pedal3;
    //ruckaMin = results[3];
    pedal1max = 0;
    pedal2max = 0;
    pedal3max = 0;
    ruckaMax = 0;
    Serial.println("Start kalibrace");
    Serial.print("Minimální hodnoty: Pedál 1:");
    Serial.print(pedal1min);
    Serial.print(" Pedál 2:");
    Serial.print(pedal2min);
    Serial.print(" Pedál 3:");
    Serial.print(pedal3min);
    Serial.print(" Ručka:");
    Serial.println(ruckaMin);
    Serial.println("Sešlápni opakovaně všechny 3 pedály + ručku na doraz do 10 sekund (brzdu dle libosti)");
    kalibraceStart = false;
  }
  if(kalibrace){
    if(pedal1 > pedal1max){
      pedal1max = pedal1;
    }
    if(pedal2 > pedal2max){
      pedal2max = pedal2;
    }
    if(pedal3 > pedal3max){
      pedal3max = pedal3;
    }
    //  if(results[3] > ruckaMax){
    //    ruckaMax = results[3];
    //  }
  }
  if (millis() > (kalibraceZacatek + 10000) && kalibrace == true){
    kalibrace = false;
    EEPROMWritelong(0,pedal1min);
    EEPROMWritelong(4,pedal1max);
    EEPROMWritelong(8,pedal2min);
    EEPROMWritelong(12,pedal2max);
    EEPROMWritelong(16,pedal3min);
    EEPROMWritelong(20,pedal3max);
    EEPROMWritelong(24,ruckaMin);
    EEPROMWritelong(28,ruckaMax);
    Serial.println("Konec kalibrace!");
    Serial.print("Maximální hodnoty: Pedál 1:");
    Serial.print(pedal1max);
    Serial.print(" Pedál 2:");
    Serial.print(pedal2max);
    Serial.print(" Pedál 3:");
    Serial.print(pedal3max);
    Serial.print(" Ručka:");
    Serial.println(ruckaMax);
  }

}



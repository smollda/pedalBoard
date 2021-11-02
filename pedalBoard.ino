#include "HX711-multi.h"
#include "Joystick.h"
#include <EEPROM.h>
// Definice vstupů pro HX711 moduly
#define CLK 11      // Arduino D2 -> HX711 SCK (všechny tři) - piny pro novou desku
#define DOUT1 10    // Arduino D4 -> HX711 DT (brzda) 
#define DOUT2 9    // Arduino D5 -> HX711 DT (plyn)
#define DOUT3 8   //  Arduino D6 -> HX711 DT (spojka)
#define DOUT4 A0   // ručka 
#define JOYSTICK_DEFAULT_REPORT_ID 0x06
long pedal1min = 0;
long pedal1max = 0;
long pedal2min = 0;
long pedal2max = 0;
long pedal3min = 0;
long pedal3max = 0;
long ruckaMin = 0;
long ruckaMax = 0;
long kalibraceZacatek;
bool kalibrace = false;
bool kalibraceStart = false;
char recievedChar;
bool newData = false;
int sensorValue = 0;  // variable to store the value coming from the sensor
int osaDopreduDozadu = 0;  
int sekvence = 0;  
int osaPravaLeva = 0;
int stupen = 0; 
int lastButtonState[11] = {0,0,0,0,0,0,0,0,0,0,0};
bool modSekvence = false; 
bool zmena = true;
byte DOUTS[3] = {DOUT1,DOUT2,DOUT3};
//byte DOUTS[1] = {DOUT1};
#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)
long int results[CHANNEL_COUNT];

//Definice , zapnutí jednotlivých os / počet tlačítek (tady 2, aby v DIview bylo možné vidět Buffered výstup)
HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
JOYSTICK_TYPE_JOYSTICK, 11, 0,
true, true, true, true, true, true,
true, true, true, true, true);

  
void setup() {
  Joystick.begin();
  Serial.begin(115200);
  Serial.println("Setup....");
  Serial.flush();
  
//  Serial.print("EEprom: ");
//  Serial.println(EEPROMReadlong(0));
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
  
  //Nastavení rozsahu jednotl. os
  Joystick.setXAxisRange(-32767, 32767);
  Joystick.setYAxisRange(-32767, 32767);
  Joystick.setZAxisRange(-32767, 32767);
  Joystick.setBrakeRange(-32767, 32767);
  Serial.println("program:");
  
}

void loop() {
osaPravaLeva = analogRead(A4); //Hodnoty pro novou desku
sekvence = analogRead(A5);
osaDopreduDozadu = analogRead(A3);
//Serial.print (osaDopreduDozadu);
//Serial.print("          ");
//Serial.print (sekvence);
//Serial.print("          ");
//Serial.print(osaPravaLeva);
//Serial.println("          ");
if (sekvence < 250){
  modSekvence = false;
}
else{
  modSekvence=true;
}
if (!modSekvence) {
 

if(osaDopreduDozadu>600 && osaPravaLeva>580){
    stupen=8; //zpátečka
  }
if(osaDopreduDozadu>600 && osaPravaLeva>490 && osaPravaLeva<520){
    stupen=1;
  }
   if(osaDopreduDozadu>600 && osaPravaLeva<420 && osaPravaLeva>350){
    stupen=3;
  }
  if(osaDopreduDozadu>600 && osaPravaLeva<290 && osaPravaLeva>240){
    stupen=5;
  }
   if(osaDopreduDozadu>600 && osaPravaLeva<200){
    stupen=7;
  }
   if(osaDopreduDozadu<400 && osaPravaLeva>490){
    stupen=2;
  }
   if(osaDopreduDozadu<400 && osaPravaLeva<420 && osaPravaLeva>350){
    stupen=4;
  }
  if(osaDopreduDozadu<400 && osaPravaLeva<290){
    stupen=6;
  }
   if(osaDopreduDozadu<600 && osaDopreduDozadu>400){
    stupen=20;
  }
for (int index = 1; index < 9; index++)
  {
    if(index == stupen) {
      int currentButtonState = 1;
      if (currentButtonState != lastButtonState[index])
      {
        Joystick.setButton(index, currentButtonState);
        lastButtonState[index] = currentButtonState;
      }
      }
      else {
       int currentButtonState = 0;
       if (currentButtonState != lastButtonState[index])
       {
          Joystick.setButton(index, currentButtonState);
          lastButtonState[index] = currentButtonState;
       }
      }    
  }
//Serial.println(stupen);
}
if(modSekvence){
  if(osaDopreduDozadu < 250){
    if (1 != lastButtonState[9])
       {
          Joystick.setButton(9, 1);
          lastButtonState[9] = 1;
       }
      
    if (0 != lastButtonState[10])
      {
        Joystick.setButton(10, 0);
        lastButtonState[10] = 0;
      }
      
    }
  if(osaPravaLeva < 250){
    if (1 != lastButtonState[10])
       {
          Joystick.setButton(10, 1);
          lastButtonState[10] = 1;
       }
      
    if (0 != lastButtonState[9])
       {
          Joystick.setButton(9, 0);
          lastButtonState[9] = 0;
       }     
  }
  if(osaDopreduDozadu > 260 && osaPravaLeva > 260) {
    Joystick.setButton(9, 0);
    lastButtonState[9] = 0;
    Joystick.setButton(10, 0);
    lastButtonState[10] = 0;
  }
}
//Serial.print(lastButtonState[0]);
//Serial.print("   ");
//Serial.print(lastButtonState[1]);
//Serial.print("   ");
//Serial.print(lastButtonState[2]);
//Serial.print("   ");
//Serial.print(lastButtonState[3]);
//Serial.print("   ");
//Serial.print(lastButtonState[4]);
//Serial.print("   ");
//Serial.print(lastButtonState[5]);
//Serial.print("   ");
//Serial.print(lastButtonState[6]);
//Serial.print("   ");
//Serial.print(lastButtonState[7]);
//Serial.print("   ");
//Serial.print(lastButtonState[8]);
//Serial.print("   ");
//Serial.print(lastButtonState[9]);
//Serial.print("   ");
//Serial.println(lastButtonState[10]);
scales.read(results); // přečtení všech 3 HX711, uložení výsledků do pole results
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
if (kalibraceStart)
{
  pedal1min = results[0];
  pedal2min = results[1];
  pedal3min = results[2];
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
  if(results[0] > pedal1max){
    pedal1max = results[0];
  }
  if(results[1] > pedal2max){
    pedal2max = results[1];
  }
  if(results[2] > pedal3max){
    pedal3max = results[2];
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
Joystick.setXAxis(constrain(prepocet(pedal1min,pedal1max,results[0]),-32767,32767));  // Brzda
//Serial.println(constrain(prepocet(pedal1min,pedal1max,results[0]),-32767,32767));
Joystick.setYAxis(constrain(prepocet(pedal2min,pedal2max,results[1]),-32767,32767));  // Plyn
Joystick.setZAxis(constrain(prepocet(pedal3min,pedal3max,results[2]),-32767,32767));  // Spojka
//Joystick.setBrake(constrain(prepocet(ruckaMin,ruckaMax,results[3]),-32767,32767));  // Ručka
//Serial.println(constrain(prepocet(ruckaMin,ruckaMax,results[3]),-32767,32767));

delay(20);  // v příkladu knihovny HX711-multi byl tenhle delay, nevím proč, tak jsem ho tu nechal
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
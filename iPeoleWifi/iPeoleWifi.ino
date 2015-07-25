#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <Time.h>
#include <avr/wdt.h>
#include <OneWire.h>



int thermoSO = 37;
int thermoCS = 33;
int thermoCLK = 39;
int POOL_TEMP_PIN = 10;
int EXTERNAL_TEMP_PIN = 11;
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoSO);
OneWire  dsPoolTemp(POOL_TEMP_PIN);
OneWire  dsExternalTemp(EXTERNAL_TEMP_PIN);  

//declare variables for the motor pins
 int Stepper1Pin1 = 8;    // Blue   - 28BYJ48 pin 1
 int Stepper1Pin2 = 9;    // Pink   - 28BYJ48 pin 2
 int Stepper1Pin3 = 10;    // Yellow - 28BYJ48 pin 3
 int Stepper1Pin4 = 11;    // Orange - 28BYJ48 pin 4

int Stepper2Pin1 = 12;    // Blue   - 28BYJ48 pin 1
int Stepper2Pin2 = 13;    // Pink   - 28BYJ48 pin 2
int Stepper2Pin3 = 14;    // Yellow - 28BYJ48 pin 3
int Stepper2Pin4 = 15;    // Orange - 28BYJ48 pin 4                       

                         

int motorSpeed = 1200;  //variable to set stepper speed
 int count = 0;          // count of steps made
 int countsperrev = 512; // number of steps per full revolution
 int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};



double m_Inner;
double m_Outter;
double m_dPositionPercent; //Position Damper
bool m_bFanRunning; // Is Fan Running
bool m_bCeilingFanRunning; //Ceiling Fan Started
double m_PoolTemperature;
double m_OutsideTemperature; 
void setup() {
  
  Serial.begin(9600);
  Wifisetup();
  wdt_enable(WDTO_8S);
}

void loop()
{
  wdt_reset();
    
  ReadTemperatureThermo();
  ReadPoolTemperature();
  UpdateThingSpeak();
  wdt_reset();
  digitalClockDisplay();
  delay(500);
}

void InitStepper()
{
  for(int i=8;i<=11;i++)
    pinMode(i, OUTPUT);
  for(int i=12;i<=15;i++)
    pinMode(i, OUTPUT);
  
  
}

void ReadTemperatureThermo()
{
   m_Inner =thermocouple.readInternal();
   m_Outter = thermocouple.readCelsius();
   Serial.print("Inner=");
   Serial.println(m_Inner);
   Serial.print("Outter=");
   Serial.println(m_Outter);  
}

void ReadPoolTemperature()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !dsPoolTemp.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    dsPoolTemp.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  dsPoolTemp.reset();
  dsPoolTemp.select(addr);
  dsPoolTemp.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = dsPoolTemp.reset();
  dsPoolTemp.select(addr);    
  dsPoolTemp.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = dsPoolTemp.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  m_PoolTemperature = celsius * 1.8 + 32.0;
  Serial.print("Pool  Temperature = ");
  Serial.print(m_PoolTemperature);
  Serial.println(" F");
}

//////////////////////////////////////////////////////////////////////////////
 //set pins to ULN2003 high in sequence from 1 to 4
 //delay "motorSpeed" between each pin setting (to determine speed)
 void anticlockwise()
 {
   for(int i = 0; i < 8; i++)
   {
     setOutput(i);
     delayMicroseconds(motorSpeed);
   }
 }

void clockwise()
 {
   for(int i = 7; i >= 0; i--)
   {
     setOutput(i);
     delayMicroseconds(motorSpeed);
   }
 }

void setOutput(int out)
 {
   digitalWrite(Stepper1Pin1, bitRead(lookup[out], 0));
   digitalWrite(Stepper1Pin2, bitRead(lookup[out], 1));
   digitalWrite(Stepper1Pin3, bitRead(lookup[out], 2));
   digitalWrite(Stepper1Pin4, bitRead(lookup[out], 3));
 }




#include <Adafruit_CC3000.h>

#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include "Time.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed


Adafruit_CC3000_Client client;

const unsigned long
  connectTimeout  = 15L * 1000L, // Max time to wait for server connection
  responseTimeout = 15L * 1000L; // Max time to wait for data from server
unsigned long
  lastPolledTime  = 0L, // Last value retrieved from time server
  sketchTime      = 0L; // CPU milliseconds since last server query


// To reduce load on NTP servers, time is polled once per roughly 24 hour period.
// Otherwise use millis() to estimate time since last query.  Plenty accurate.
void SetLocalTime(void) {
  unsigned long t  = getTime(); // Query time server

    
  if(t)
  {                       // Success?
    lastPolledTime = t;         // Save time
    sketchTime     = millis();  // Save sketch time of last valid time query
  }


  unsigned long currentTime = lastPolledTime + (millis() - sketchTime) / 1000;
  setTime(currentTime);
  digitalClockDisplay();
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" UTC ");
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year()); 
  Serial.println(); 
}

// Minimalist time server query; adapted from Adafruit Gutenbird sketch,
// which in turn has roots in Arduino UdpNTPClient tutorial.
unsigned long getTime(void) {

  uint8_t       buf[48];
  unsigned long ip, startTime, t = 0L;

  Serial.print(F("Locating time server..."));

  // Hostname to IP lookup; use NTP pool (rotates through servers)
  if(cc3000.getHostByName("pool.ntp.org", &ip)) {
    static const char PROGMEM
      timeReqA[] = { 227,  0,  6, 236 },
      timeReqB[] = {  49, 78, 49,  52 };

    Serial.println(F("\r\nAttempting connection..."));
    startTime = millis();
    do {
      client = cc3000.connectUDP(ip, 123);
    } while((!client.connected()) &&
            ((millis() - startTime) < connectTimeout));

    if(client.connected()) {
      Serial.print(F("connected!\r\nIssuing request..."));

      // Assemble and issue request packet
      memset(buf, 0, sizeof(buf));
      memcpy_P( buf    , timeReqA, sizeof(timeReqA));
      memcpy_P(&buf[12], timeReqB, sizeof(timeReqB));
      client.write(buf, sizeof(buf));

      Serial.print(F("\r\nAwaiting response..."));
      memset(buf, 0, sizeof(buf));
      startTime = millis();
      while((!client.available()) &&
            ((millis() - startTime) < responseTimeout));
      if(client.available()) {
        client.read(buf, sizeof(buf));
        t = (((unsigned long)buf[40] << 24) |
             ((unsigned long)buf[41] << 16) |
             ((unsigned long)buf[42] <<  8) |
              (unsigned long)buf[43]) - 2208988800UL;
        Serial.print(F("OK\r\n"));
      }
      client.close();
    }
  }
  if(!t) Serial.println(F("error"));
  return t;
}

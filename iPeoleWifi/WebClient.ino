/*************************************************** 
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /*
This example does a test of the TCP client capability:
  * Initialization
  * Optional: SSID scan
  * AP connection
  * DHCP printout
  * DNS lookup
  * Optional: Ping
  * Connect to website and print out webpage contents
  * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/

#define WLAN_SSID       "TurcotteDemers"           // cannot be longer than 32 characters!
#define WLAN_PASS       "salut123"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

// What page to grab!
#define WEBSITE      "api.thingspeak.com"
#define WEBPAGE      "/testwifi/index.html"

char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "IGNBSQ94GSJ7651R";

const int updateThingSpeakInterval = 80 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)


// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;



Adafruit_CC3000_Client www;
/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;

bool Wifisetup(void)
{
  
   
  /* Initialise the module */
  Serial.println(F("\nInitializing CC3000..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  wdt_reset();
  // Optional SSID scan
  // listSSIDResults();
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    
  }
wdt_reset();
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  //Serial.println(F("Request DHCP"));
  //while (!cc3000.checkDHCP())
  //{
  //  delay(100); // ToDo: Insert a DHCP timeout!
  //}
  
  /*
  uint32_t ipAddress = cc3000.IP2U32(192, 168, 1, 20);
  uint32_t netMask = cc3000.IP2U32(255, 255, 255, 0);
  uint32_t defaultGateway = cc3000.IP2U32(192, 168, 1, 1);
  uint32_t dns = cc3000.IP2U32(8, 8, 4, 4);
  if (!cc3000.setStaticIPAddress(ipAddress, netMask, defaultGateway, dns)) {
  Serial.println(F("Failed to set static IP!"));
  while(1);
  }
*/

  

  /* Display the IP address DNS, Gateway, etc. */  
  int iCount =0;
  
  while (! displayConnectionDetails() && iCount < 10 ) {
    delay(1000);
    wdt_reset();
    iCount++;
  }
  
   
  // Try looking up the website's IP address
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      wdt_reset();
      Serial.println(F("Couldn't resolve!"));
    }

    delay(500);
  }

  cc3000.printIPdotsRev(ip);
  
  
  
  return true;
  
}

void Wifiloop(void)
{
  
  wdt_reset();
  String tsData = "field1="+String(m_Outter)+"&field2="+String(m_Inner)+"&field3="+String(m_dPositionPercent)+"&field4="+String(m_bFanRunning)+"&field5="+String(m_bCeilingFanRunning)+"&field6="+String(m_PoolTemperature)+"&field7="+String(m_OutsideTemperature);
  
  if(!www.connected())
  {
      Serial.println("Connecting to ThingSpeak...");
      www = cc3000.connectTCP(ip, 80);
      Serial.println("After Connecting to ThingSpeak...");
  }


  if (www.connected())
  {
    wdt_reset();
    Serial.println("Before print1 to ThingSpeak...");
    www.print("POST /update HTTP/1.1\n");
    delay(200);
    www.print("Host: api.thingspeak.com\n");
    delay(200);
    www.print("Connection: close\n");
    delay(200);
    www.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    delay(200);
    www.print("Content-Type: application/x-www-form-urlencoded\n");
    delay(200);
    www.print("Content-Length: ");
    delay(200);
    www.print(tsData.length());
    delay(200);
    www.print("\n\n");
    delay(200);
    www.print(tsData);
    Serial.println("After print All to ThingSpeak...");
    
    
   lastConnectionTime = millis();
            
    
    Serial.println(F("Reading answer..."));
    while (client.connected())
    {
      while (client.available())
      {
        char c = client.read();
        Serial.print(c);
      }
    }
    
    Serial.println(F("Disconnecting from Thingspeak"));
    
    
  }
  else
  {
    Serial.println(F("Connection failed"));    
    
  
    
   
    Serial.println();
   
  }
  
  
}

void UpdateThingSpeak(void)
{
   if((millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    
      Wifiloop();
      
     
  }
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint32_t index;
  uint8_t valid, rssi, sec;
  char ssidname[33]; 

  //if (!cc3000.startSSIDscan(&index)) {
    //Serial.println(F("SSID scan failed!"));
    //return;
  //}

  //Serial.print(F("Networks found: ")); //Serial.println(index);
  //Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    //Serial.print(F("SSID Name    : ")); //Serial.print(ssidname);
    //Serial.println();
    //Serial.print(F("RSSI         : "));
    //Serial.println(rssi);
    //Serial.print(F("Security Mode: "));
    //Serial.println(sec);
    //Serial.println();
  }
  //Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}



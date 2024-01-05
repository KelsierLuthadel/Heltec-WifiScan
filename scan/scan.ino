#include "Arduino.h"
#include "WiFi.h"
#include "images.h"
#include "display.h"
#include "scan-wifi.h"

// Create a screen. Setting the parameter to true also outputs text to the serial port )
Display screen(true);

// Enable the WiFi adapter, all output is sent to the screen
WiFiScan wifi(&screen);

// Sleep flags
bool resendflag=false;
bool deepsleepflag=false;
bool interrupt_flag = false;

// GPIO 0 (Flash Button)
void interrupt_GPIO0()
{
	interrupt_flag = true;
}

void interrupt_handle(void)
{
	if(interrupt_flag)
	{
    Serial.println("******************* SLEEP *************************");
		interrupt_flag = false;
		if(digitalRead(0)==0)
		{
			deepsleepflag=true;
		}
	}
}

// Turn the onboard LED on
void ledOn()
{
  pinMode(LED ,OUTPUT);
	digitalWrite(LED, HIGH);  
}

// Turn the onboard LED off
void ledOff()
{
  pinMode(LED ,OUTPUT);
	digitalWrite(LED, LOW);  
}

// Turn on Vext power supply
void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
  
}

// Turn off Vext power supply
void VextOFF(void) 
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

//The chip ID is essentially its MAC address(length: 6 bytes).
String getMac()
{
  uint64_t chipid=ESP.getEfuseMac();
  return "MAC:" + String(chipid, HEX); 
}

void setup()
{
	Serial.begin(115200);

  //OLED use Vext power supply, Vext must be turned ON before OLED initialition.
	VextON();
	delay(100);

  // Initialise the display and show the logo  
  screen.init();
  screen.clear();
  screen.logo();

	delay(2000);
  screen.clear();
	
  // Disconnect from any previous WiFi connections
  wifi.disconnect();
	  
  screen.outputln(0, 0, getMac(), true);

  delay(2000);

  screen.clear();

	attachInterrupt(0, interrupt_GPIO0, FALLING);
}

void loop()
{
  interrupt_handle();

  wifi.scan(false, true);

  if(deepsleepflag)
  {
	  VextOFF();
	  esp_sleep_enable_timer_wakeup(600*1000*(uint64_t)1000);
	  esp_deep_sleep_start();
  }
}

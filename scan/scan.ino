#include "Arduino.h"
#include "WiFi.h"
#include "images.h"
#include "display.h"
#include "scan-wifi.h"

Display screen(true);
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

void ledOn()
{
  pinMode(LED ,OUTPUT);
	digitalWrite(LED, HIGH);  
}

void ledOff()
{
  pinMode(LED ,OUTPUT);
	digitalWrite(LED, LOW);  
}

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
  
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

void setup()
{
	Serial.begin(115200);
	VextON();
	delay(100);
  screen.init();
  screen.clear();
  screen.logo();

	delay(2000);
  screen.clear();
	
  wifi.disconnect();
	  
	uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).

  String macAddress = "MAC:" + String(chipid, HEX);  
  screen.outputln(0, 0, macAddress, true);

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

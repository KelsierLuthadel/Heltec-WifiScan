#include "Arduino.h"
#include "WiFi.h"
#include "images.h"
#include <Wire.h>  
#include "HT_SSD1306Wire.h"

/********************************* lora  *********************************************/

SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst
bool resendflag=false;
bool deepsleepflag=false;
bool interrupt_flag = false;

void logo(){
	factory_display.clear();
	factory_display.drawXbm(0,5,logo_width,logo_height,(const unsigned char *)logo_bits);
	factory_display.display();
}

void WIFIConnect(const char* BSSID, const char* password)
{
  WiFi.setAutoConnect(true);
	WiFi.begin(BSSID,password);
	delay(100);

	byte count = 0;
	while(WiFi.status() != WL_CONNECTED && count < 10)
	{ 
		count ++;
		delay(500);
		output(0, 0, "Connecting...");
		factory_display.display();
	}

	factory_display.clear();
	if(WiFi.status() == WL_CONNECTED)
	{
		output(0, 0, "Connecting...OK.");
		factory_display.display();
	}
	else
	{
		factory_display.clear();
		output(0, 0, "Connecting...Failed");
		factory_display.display();
	}

	output(0, 10, "WIFI Setup done");
	factory_display.display();
	delay(500);
  factory_display.clear();
}

void WIFISetUp(void)
{
	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.disconnect(true);
	delay(100);
	WiFi.mode(WIFI_STA);
	
}

void WIFIScan()
{
  int networksFound = WiFi.scanNetworks();

  if (networksFound == 0)
  {
    factory_display.clear();
    output(0, 0, "no network found");
    factory_display.display();
  }
  else
  {
    outputln(0, 0, "Networks: " + (String)networksFound);
    factory_display.display();

    int y = 0;
    for (int i = 0; i < networksFound; ++i, y++) {
    // Print SSID and RSSI for each network found
      unsigned int position = i + 1;

      output(0, (y+1)*9, (String)(position));
      output(6, (y+1)*9, ":");
      
      String ssid = (String)WiFi.SSID(i);
      ssid = ssid.substring(0,15);

      output(12,(y+1)*9, ssid);
      // factory_display.drawString(90,(y+1)*9, " (");
      // factory_display.drawString(98,(y+1)*9, (String)(WiFi.RSSI(i)));
      // factory_display.drawString(114,(y+1)*9, ")");
      outputln(120,(y+1)*9, (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?"  ":" *");
      delay(10);
      
      if (y > 5 && position < networksFound) 
      {
        factory_display.display();
        delay(3000);
        factory_display.clear();
        outputln(0, 0, "Networks: " + (String)networksFound);
        y=0;
        
      }
    }

		factory_display.display();
		delay(3000);
		factory_display.clear();
	}
}

void output(unsigned int x, unsigned int y, String value)
{
  factory_display.drawString(x, y, value);
  Serial.print(value);
}

void outputln(unsigned int x, unsigned int y, String value)
{
  factory_display.drawString(x, y, value);
  Serial.println(value);
}


void interrupt_GPIO0()
{
	interrupt_flag = true;
}
void interrupt_handle(void)
{
	if(interrupt_flag)
	{
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

	factory_display.init();
	factory_display.clear();
	factory_display.display();
	logo();

	delay(2000);
	factory_display.clear();

	WIFISetUp();
	  
	uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).

  String macAddress = "MAC:" + String(chipid, HEX);  
  outputln(0, 0, macAddress);
  factory_display.display();

  delay(2000);

  factory_display.clear();

	attachInterrupt(0, interrupt_GPIO0, FALLING);
}

void loop()
{
  interrupt_handle();
	WIFIScan();
  if(deepsleepflag)
  {
	  VextOFF();
	  esp_sleep_enable_timer_wakeup(600*1000*(uint64_t)1000);
	  esp_deep_sleep_start();
  }
}

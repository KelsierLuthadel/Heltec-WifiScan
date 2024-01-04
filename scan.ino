#include "Arduino.h"
#include "WiFi.h"
#include "images.h"
#include <Wire.h>  
#include "HT_SSD1306Wire.h"

/********************************* lora  *********************************************/

SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst


void logo(){
	factory_display.clear();
	factory_display.drawXbm(0,5,logo_width,logo_height,(const unsigned char *)logo_bits);
	factory_display.display();
}

void WIFISetUp(void)
{
	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.disconnect(true);
	delay(100);
	WiFi.mode(WIFI_STA);
	WiFi.setAutoConnect(true);
	WiFi.begin("Your WiFi SSID","Your Password");//fill in "Your WiFi SSID","Your Password"
	delay(100);

	byte count = 0;
	while(WiFi.status() != WL_CONNECTED && count < 10)
	{
		count ++;
		delay(500);
		factory_display.drawString(0, 0, "Connecting...");
		factory_display.display();
	}

	factory_display.clear();
	if(WiFi.status() == WL_CONNECTED)
	{
		factory_display.drawString(0, 0, "Connecting...OK.");
		factory_display.display();
	}
	else
	{
		factory_display.clear();
		factory_display.drawString(0, 0, "Connecting...Failed");
		factory_display.display();
	}

	factory_display.drawString(0, 10, "WIFI Setup done");
	factory_display.display();
	delay(500);
}

void WIFIScan()
{
  int n = WiFi.scanNetworks();

  if (n == 0)
  {
    factory_display.clear();
    factory_display.drawString(0, 0, "no network found");
    factory_display.display();
  }
  else
  {
    factory_display.drawString(0, 0, (String)n);
    factory_display.drawString(14, 0, "networks found:");
    factory_display.display();

    int y = 0;
    for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
      factory_display.drawString(0, (y+1)*9,(String)(i + 1));
      factory_display.drawString(6, (y+1)*9, ":");
      
      String ssid = (String)WiFi.SSID(i);
      ssid = ssid.substring(0,15);

      factory_display.drawString(12,(y+1)*9, ssid);
      // factory_display.drawString(90,(y+1)*9, " (");
      // factory_display.drawString(98,(y+1)*9, (String)(WiFi.RSSI(i)));
      // factory_display.drawString(114,(y+1)*9, ")");
      factory_display.drawString(120,(y+1)*9, (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?"  ":" *");
      delay(10);
      y++;
      if (y>5) 
      {
        factory_display.display();
        delay(3000);
        factory_display.clear();
        factory_display.drawString(0, 0, (String)n);
        factory_display.drawString(14, 0, "networks found:");
        y=0;
        
      }
    }

		factory_display.display();
		delay(3000);
		factory_display.clear();
	}
}

bool resendflag=false;
bool deepsleepflag=false;
bool interrupt_flag = false;
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
	WiFi.disconnect(); 
	WiFi.mode(WIFI_STA);
	delay(100);

  // WiFiInit(1);

	uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
	Serial.printf("ESP32ChipID=%04X",(uint16_t)(chipid>>32));//print High 2 bytes
	Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.

	attachInterrupt(0,interrupt_GPIO0,FALLING);
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

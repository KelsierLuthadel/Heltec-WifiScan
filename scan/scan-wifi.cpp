#include "scan-wifi.h"
#include "Arduino.h"
#include "WiFi.h"


WiFiScan::WiFiScan(Display* display)
{
  setMode(WIFI_STA);

  this->autoConnect = false;
  this->display = display;
}

WiFiScan::WiFiScan(const char* bssid, const char* password, Display* display)
{
  setMode(WIFI_STA);

  this->setBSSID(bssid);
  this->setPassword(password);

  this->autoConnect = false;
  this->display = display;
}

bool WiFiScan::connect()
{
  WiFi.setAutoConnect(this->autoConnect);
	WiFi.begin(this->bssid,this->password);
	delay(100);

  byte count = 0;
	while(WiFi.status() != WL_CONNECTED && count < 10)
	{ 
		count ++;
		delay(500);
    display->output(0, 0, "Connecting...", true);
	}

	if(WiFi.status() == WL_CONNECTED)
	{
		display->output(0, 0, "Connecting...OK.", true);
	}
	else
	{
    display->clear();
    display->output(0, 0, "Connecting...Failed", true);
    return false;
	}

	display->output(0, 10, "WIFI Setup done", true);
	delay(500);
  display->clear();
  return true;
}

bool WiFiScan::connect(const char* bssid, const char* password)
{
  this->setBSSID(bssid);
  this->setPassword(password);

  return this->connect();
}

void WiFiScan::disconnect()
{
  WiFi.disconnect(true);
  delay(100);
}

void WiFiScan::scan(bool showStrength, bool showChannel)
{
  int networksFound = WiFi.scanNetworks();
  display->clear();

  if (networksFound == 0)
  {
   display->output(0, 0, "no network found", true);
  }
  else
  {
    display->outputln(0, 0, "Networks: " + (String)networksFound);

    int y = 1;

    for (int i = 0; i < networksFound; ++i) 
    {
      if (y > 6) 
      {
        display->render();
        delay(3000);
        display->clear();
        display->outputln(0, 0, "Networks: " + (String)networksFound);
        y=0;
      }

      unsigned int position = i+1;
      
      String ssid = (String)WiFi.SSID(i).substring(0,15);
      String open = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "  ": " *";
      String network = (String)position + ":" + ssid ;

      if (showChannel)
      {
        network += " CH: " + (String)(WiFi.channel(i));
      }

      if (showStrength)
      {
        network += " (" + (String)(WiFi.RSSI(i)) + ")";
      }

      network += open;

      display->outputln(0, (y+1)*9, network);
      delay(10);
      y++;
    }

    display->render();
		delay(3000);
		display->clear();
	}
}

void WiFiScan::setMode(wifi_mode_t type)
{
  WiFi.mode(type);
}

void WiFiScan::setAutoConnect(bool autoConnect)
{
  this->autoConnect = autoConnect;
}

boolean WiFiScan::setBSSID(const char* bssid)
{
  if(bssid && *bssid != 0x00 && strlen(bssid) <= 32) {
    if (NULL != this->bssid)
    {
      delete []this->bssid;
    }

    this->bssid = new char[strlen(bssid) + 1];
    strncpy(this->bssid, bssid, 32);
  }
}

boolean WiFiScan::setPassword(const char* password)
{
  if(password && strlen(password) <= 64) {
    if (NULL != this->password)
    {
        delete []this->password;
    }

    this->password = new char[strlen(password) + 1];
    strncpy(this->password, password, 64);
  }
}

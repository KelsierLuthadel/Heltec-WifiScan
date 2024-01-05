#ifndef _KELSIERWIFI_H_
#define _KELSIERWIFI_H_

#include "Arduino.h"
#include "display.h"

class WiFiScan
{

public:
  WiFiScan(Display* display);
  WiFiScan(const char* bssid, const char* password, Display* display);

  bool connect();
  bool connect(const char* bssid, const char* password);
  void disconnect();

  void scan(bool showStrength = false, bool showChannel = false);

  void setAutoConnect(bool autoConnect);
  boolean setBSSID(const char* bssid);
  boolean setPassword(const char* password);

protected:
  bool autoConnect;
  char *bssid;
  char *password;
  Display* display;

};

#endif
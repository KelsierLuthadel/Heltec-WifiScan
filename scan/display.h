#ifndef _KELSIERDISPLAY_H_
#define _KELSIERDISPLAY_H_

#include "Arduino.h"

class Display
{

public:
  Display(bool serial = false);
  void init();
  void setSerialOutput(bool serial);
  void clear();
  void render();

  void output(unsigned int x, unsigned int y, String value, bool render=false);
  void outputln(unsigned int x, unsigned int y, String value, bool render=false);

  void logo();

protected:
  bool serialOutput;

};

#endif
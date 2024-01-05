#include "display.h"
#include "Arduino.h"
#include <Wire.h>  
#include "HT_SSD1306Wire.h"
#include "images.h"

SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst

Display::Display(bool serial)
{
  this->serialOutput = serial;
}

void Display::init()
{
  display.init();
}

void Display::setSerialOutput(bool serial)
{
  this->serialOutput = serial;
}

void Display::clear()
{
  display.clear();
}

void Display::render()
{
  display.display();
}

void Display::logo()
{
  display.clear();
	display.drawXbm(0,5,logo_width,logo_height,(const unsigned char *)logo_bits);
	display.display();
}

// Output data to the display and serial port
void Display::output(unsigned int x, unsigned int y, String value, bool render)
{
  display.drawString(x, y, value);
  
  if (render)
  {
    this->render();
  }

  if (serialOutput)
  {
    Serial.print(value);
  }
}

// Output data to the display and serial port (with newline)
void Display::outputln(unsigned int x, unsigned int y, String value, bool render)
{
  display.drawString(x, y, value);

  if (render)
  {
    this->render();
  }

  if (serialOutput)
  {
    Serial.println(value);
  }
}

#pragma once

#include "CommandReader.h"
#include "CommandReaderInstance.h"

class DeviceControl {
public:
  DeviceControl(Adafruit_NeoPixel &strip, CommandReader &cr, char const *name)
    : _strip(strip), _cr(cr), _command(0), _counter(0), _name(name)
  {
    for (int i = 0; i < _maxNumParams; ++i) {
      _par[i] = -1;
    }
  }

  void parse() {
    String commandName = _cr.readCommandName();
#if DEBUG
    _("Read command '"); _(commandName); _("' "); _("  with parameters: ");
#endif
    for (int i = 0; i < _maxNumParams; ++i) {
      if (_cr.nextIsCommand()) {
        _par[i] == 0;
      } else {
        _par[i] = _cr.readInt();
#if DEBUG
        _(_par[i]); _(" ");
#endif
      }
    }
#if DEBUG
    _n("");
#endif
    for (_command = 0; _command < _numCommands; ++_command) {
      if (commandName == _commandNames[_command]) break;
    }
    _("Command number set to "); _(_command); _n(".");
    String endName = _cr.readCommandName();
    if (endName != "END") {
      _("[");_(_name);_("][ERROR]: Command doesn't end with [END] but with ["); _(endName); _n("].");
      _command = 0; // Turn the light off due to erroneous command
    } else if (_command == _numCommands) {
      _("[");_(_name);_("][ERROR]: command '"); _(commandName); _n("' not found.");
      _command = 0; // Turn the light off due to erroneous command
    } else {
      _("["); _(_name); _n("][OK]");
      _counter = 0;
      _millisStart = millis();
    }
    cr.gobbleTrailingSpaces();
  }

  void handle() {
    switch (_command) {
    case 0: _off(); break;
    case 1: _color(_par[0], _par[1], _par[2]); break;
    case 2: _colors(_par[0], _par[1], _par[2]); break;
    case 3: _carousel(_par[0], _par[1], _par[2], _par[3]); break;
    case 4: _random(_par[0], _par[1]); break;
    }
    _counter += 1; 
    if (_counter == 0) {
      _counter = 1; // Counter may never become 0 again
    }
  }
private:
  void _off() {
    _color(0, 0, 0);
    _strip.show();
  }

  void _color(unsigned char r, unsigned char g, unsigned char b) {
    for (int i = 0; i < _strip.numPixels(); ++i) {
      _strip.setPixelColor(i, Adafruit_NeoPixel::Color(r, g, b));
    }
    _strip.show();
  }

  void _colors(unsigned char intensity, long int period, unsigned int phase) {
    long int t = millis() - _millisStart;
    unsigned int wheelPos = period ? (t << 16) / period : 0;
    wheelPos += phase;
    long int color = _wheel(wheelPos, intensity);
    for (int i = 0; i < _strip.numPixels(); ++i) {
      _strip.setPixelColor(i, color);
    }
    _strip.show();
  }

  void _carousel(unsigned char intensity, long int period, unsigned int phase, bool reverseDirection) {
    long int t = millis() - _millisStart;
    unsigned int wheelPos = (t << 16) / period;
    wheelPos += phase;
    for (long int i = 0; i < _strip.numPixels(); ++i) {
      unsigned int wheelPos2 = wheelPos + (i << 16) / _strip.numPixels();
      if (reverseDirection) {
        wheelPos2 = 0xFFFF - wheelPos2;
      }
      long int color = _wheel(wheelPos2, intensity);
      _strip.setPixelColor(i, color);
    }
    _strip.show();
  }

  void _random(unsigned char intensity, long int period) {
    _strip.show();
  }

  uint32_t _wheel(unsigned int wheelPos, unsigned char intensity) {
    if (wheelPos < 21845) {
      long int r = 0xFFFF - wheelPos * 3;
      long int b = wheelPos * 3;
      r *= intensity; r >>= 16;
      b *= intensity; b >>= 16;
      return Adafruit_NeoPixel::Color(r, 0, b);
    }
    if (wheelPos < 43690) {
      wheelPos -= 21845;
      long int g = wheelPos * 3;
      long int b = 0xFFFF - wheelPos * 3;
      g *= intensity; g >>= 16;
      b *= intensity; b >>= 16;
      return Adafruit_NeoPixel::Color(0, g, b);
    }
    wheelPos -= 43690;
    long int r = wheelPos * 3;
    long int g = 0xFFFF - wheelPos * 3;
    r *= intensity; r >>= 16;
    g *= intensity; g >>= 16;
    return Adafruit_NeoPixel::Color(r, g, 0);
  }


  static const int _maxNumParams = 8;
  static const int _numCommands = 5;
  static char const *_commandNames[5];

  CommandReader &_cr;
  Adafruit_NeoPixel &_strip;
  long int _par[_maxNumParams];
  int _command;
  unsigned long int _counter;
  unsigned long int _millisStart;
  char const *_name;
};

char const *DeviceControl::_commandNames[DeviceControl::_numCommands] = {
  "off",
  "color",
  "colors",
  "carousel",
  "random"
};


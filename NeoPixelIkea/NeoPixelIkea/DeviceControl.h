#pragma once

#include "CommandReader.h"
#include "CommandReaderInstance.h"

class DeviceControl {
public:
	DeviceControl(Adafruit_NeoPixel &strip, CommandReader &cr)
		: _strip(strip), _cr(cr), _command(0), _counter(0)
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
				_par[i] == -1;
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
		if (endName != "end") {
			_("[ERROR]: Command doesn't end with [END] but with ["); _(endName); _n("].");
			_command = 0; // Turn the light off due to erroneous command
		} else if (_command == _numCommands) {
			_("[ERROR]: command '"); _(commandName); _n("' not found.");
			_command = 0; // Turn the light off due to erroneous command
		} else {
			Serial.println("[OK]");
			_counter = 0;
			_millisStart = millis();
		}
	}

	void handle() {
		switch (_command) {
		case 0: _off(); break;
		case 1: _color(_par[0], _par[1], _par[2]); break;
		case 2: _colors(_par[0], _par[1]); break;
		case 3: _carousel(_par[0], _par[1]); break;
		case 4: _random(_par[0], _par[1]); break;
		}
		_counter += 1; 
		if (_counter == 0) {
			_counter = 1; // Counter may never become 0 again
		}
	}
private:
	void _off() {
		_color(255, 255, 255);
		_strip.show();
	}

	void _color(unsigned char r, unsigned char g, unsigned char b) {
		for (int i = 0; i < _strip.numPixels(); ++i) {
			_strip.setPixelColor(i, Adafruit_NeoPixel::Color(r, g, b));
		}
		_strip.show();
	}

	void _colors(unsigned char intensity, long int period) {
		long int t = millis() - _millisStart;
		unsigned char wheelPos = t * 255 / period;
		long int color = _wheel(wheelPos, intensity);
		for (int i = 0; i < _strip.numPixels(); ++i) {
			_strip.setPixelColor(i, color);
		}
		_strip.show();
	}

	void _carousel(unsigned char intensity, long int period) {
		long int t = millis() - _millisStart;
		unsigned char wheelPos = t * 255 / period;
		long int color = _wheel(wheelPos, intensity);
		for (long int i = 0; i < _strip.numPixels(); ++i) {
			unsigned char wheelPos = (i * 255 / period) * (t * 255 / period);
			long int color = _wheel(wheelPos, intensity);
			_strip.setPixelColor(i, color);
		}
		_strip.show();
	}

	void _random(unsigned char intensity, long int period) {
		long int t = millis() - _millisStart;
		unsigned char wheelPos = t * 255 / period;
		long int color = _wheel(wheelPos, intensity);
		for (long int i = 0; i < _strip.numPixels(); ++i) {
			unsigned char wheelPos = (i * 255 / period) * (t * 255 / period);
			long int color = _wheel(wheelPos, intensity);
			_strip.setPixelColor(i, color);
		}
		_strip.show();
	}

	uint32_t _wheel(unsigned char wheelPos, unsigned char intensity) {
		wheelPos = 255 - wheelPos;
		if (wheelPos < 85) {
			long int r = 255 - wheelPos * 3;
			long int b = wheelPos * 3;
			r *= intensity; r /= 255;
			b *= intensity; b /= 255;
			return Adafruit_NeoPixel::Color(r, 0, b);
		}
		if (wheelPos < 170) {
			wheelPos -= 85;
			long int g = wheelPos * 3;
			long int b = 255 - wheelPos * 3;
			g *= intensity; g /= 255;
			b *= intensity; b /= 255;
			return Adafruit_NeoPixel::Color(0, g, b);
		}
		wheelPos -= 170;
		long int r = wheelPos * 3;
		long int g = 255 - wheelPos * 3;
		r *= intensity; r /= 255;
		g *= intensity; g /= 255;
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
};

char const *DeviceControl::_commandNames[DeviceControl::_numCommands] = {
	"off",
	"color",
	"colors",
	"carousel",
	"random"
};


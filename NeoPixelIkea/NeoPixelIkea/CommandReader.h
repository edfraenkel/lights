#pragma once

class CommandReader {
public:
	CommandReader(Stream &s) : _s(s) {};

	template<typename A>
	inline void _(A const &x) {
		_s.print(x);
	};

	template<typename A>
	inline void _n(A const &x) {
		_s.println(x);
	};

	inline bool nextIsCommand() {
		_gotoNextToken();
		return _s.peek() == '[';
	}

	inline String readCommandName() {
		_gotoNextToken();
		_readChar();
		return _s.readStringUntil(']');
	}

	inline long int readInt() {
		_gotoNextToken();
		return Serial.parseInt();
	}

	bool inputAvailable() {
		return _s.available();
	}

private:
		
	inline void _gotoNextToken() {
		while (isWhitespace(_s.peek())) {
			int c = _s.read();
			if (c == -1) {
				_n("[ERROR]: Timeout.");
			}
		}
	}

	inline void _assertRead(char c) {
		char r = _s.read();
		if (c == -1) {
			_n("[ERROR]: Timeout.");
		}
		else if (r != c) {
			_("[ERROR]: Expected "); _(c); _(" but got "); _(r); _n(" instead.");
		}
	}

	inline char _readChar() {
		int c = _s.read();
		if (c == -1) {
			_n("[ERROR]: Timeout");
		}
		return c;
	}

	Stream &_s;
};


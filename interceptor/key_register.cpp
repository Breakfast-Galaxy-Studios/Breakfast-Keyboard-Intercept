#include <iostream>
#include <main.h>
using namespace std;

int newCode(int code) {
	switch (code) {
		case 82:
		case 71: 
		case 73:
		case 83:
		case 79:
		case 81:
		case 72:
		case 75:
		case 80:
		case 77:
			return code + 100;
		default: return code;
	}
}

string returnKeycode(int code) {
	switch (code){
		// Function Keys
		case 1: return "esc";
		case 59: return "F1";
		case 60: return "F2";
		case 61: return "F3";
		case 62: return "F4";
		case 63: return "F5";
		case 64: return "F6";
		case 65: return "F7";
		case 66: return "F8";
		case 67: return "F9";
		case 68: return "F10";
		case 87: return "F11";
		case 88: return "F12";

		// Top row (numbers)
		case 41: return "`";
		case 2: return "1";
		case 3: return "2";
		case 4: return "3";
		case 5: return "4";
		case 6: return "5";
		case 7: return "6";
		case 8: return "7";
		case 9: return "8";
		case 10: return "9";
		case 11: return "0";
		case 12: return "-";
		case 13: return "=";
		case 14: return "backspace";

		// Top row (qwerty)
		case 15: return "tab";
		case 16: return "q";
		case 17: return "w";
		case 18: return "e";
		case 19: return "r";
		case 20: return "t";
		case 21: return "y";
		case 22: return "u";
		case 23: return "i";
	 	case 24: return "o";
		case 25: return "p";
		case 26: return "[";
		case 27: return "]";
		case 43: return "\\";
		
		// Home row (asdfg)
		case 58: return "caps";
		case 30: return "a";
		case 31: return "s";
		case 32: return "d";
		case 33: return "f";
		case 34: return "g";
		case 35: return "h";
		case 36: return "j";
		case 37: return "k";
		case 38: return "l";
		case 39: return ";";
		case 40: return "'";
		case 28: return "enter";

		// Bottom row (zxcvb)
		case 42: return "lshift";
		case 44: return "z";
		case 45: return "x";
		case 46: return "c";
		case 47: return "v";
		case 48: return "b";
		case 49: return "n";
		case 50: return "m";
		case 51: return ",";
		case 52: return ".";
		case 53: return "/";
		case 54: return "rshift";

		// Last row (ctrl, meta, alt, space)
		case 29: return "ctrl";
		case 56: return "alt";
		case 57: return "space";
		case 91: return "meta";
		case 93: return "context";

		// Center of kb (ins, home, pause, del, prtsc, pause, arrows, etc.)
		case 155: return "prtsc";
		case 170: return "scroll";
		case 169: return "pause";
		case 182: return "ins";
		case 171: return "home";
		case 173: return "pgup";
		case 183: return "del";
		case 179: return "end";
		case 181: return "pgdown";
		case 172: return "up";
		case 175: return "left";
		case 180: return "down";
		case 177: return "right";

		// Numpad
		case 76: return "num5";
		case 74: return "num-";
		case 78: return "num+";
		case 55: return "num*";
		case 70: return "scroll";
		case 69: return "numlock";
		case 82: return "num0";
		case 71: return "num7";
		case 73: return "num9";
		case 83: return "num.";
		case 79: return "num1";
		case 81: return "num3";
		case 72: return "num8";
		case 75: return "num4";
		case 80: return "num2";
		case 77: return "num6";

		// f13-f24 are shifted f1-f12


		// All undefined or non working keys
	 	default: return "undefined";
	}
}
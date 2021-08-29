#include <iostream>
#include <main.h>
using namespace std;

/**
* Used to add 100 to middle keys if numlock is on.
*/
int newCode(int code) {
	switch (code) {
		case 55:
		case 69:
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
			return (code + 100);
		default: return code;
	}
}

// This returns an int to be sent over UDP
/**
* Returns the int to be sent over UDP to soundboard. 
* Most keys used standarized scancodes that can be translated back in java, but java
* uses a few differenct codes, mostly for E0 keys.
* The hook we are using in java also doesn't normally separate numpad numbers and normal numbers
* so we made our own codes for that.
*/
int returnKeyInt(int code) {
	switch (code) {
		// All of these are "action keys" in java (minus scroll lock)
		case 91: return 3675;
		case 93: return 3677;
		case 155: return 3639;
		case 170: return 70;
		case 169: return 3653;
		case 182: return 3666;
		case 171: return 3655;
		case 173: return 3657;
		case 183: return 3667;
		case 179: return 3663;
		case 181: return 3665;
		case 172: return 57416;
		case 175: return 57419;
		case 180: return 57424;
		case 177: return 57421;

		// f13-f24 are shifted f1-f12
		case 159: return 91;
		case 160: return 92;
		case 161: return 93;
		case 162: return 99;
		case 163: return 100;
		case 164: return 101;
		case 165: return 102;
		case 166: return 103;
		case 167: return 104;
		case 168: return 105;
		case 187: return 106;
		case 188: return 107;

		// Numpad keys need special code in java
		case 76: 
		case 74: 
		case 78: 
		case 55:
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
			return (code + 1000);
		// All other keys return their normal scancodes for java
		default: return code;
	}
}

// This function is depricated and only used for debugging purposes, this shouldn't be called or refrenced in final release
/**
* Returns the string representation of the keycode.
*/
string returnKeyString(int code) {
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

		// Center of kb (ins, home, pause, del, prtsc, pause, arrows, etc.)
		// When numlock is on, these act as shifted versions of themselves
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

		

		// f13-f24 are shifted f1-f12
		case 159: return "F13";
		case 160: return "F14";
		case 161: return "F15";
		case 162: return "F16";
		case 163: return "F17";
		case 164: return "F18";
		case 165: return "F19";
		case 166: return "F20";
		case 167: return "F21";
		case 168: return "F22";
		case 187: return "F23";
		case 188: return "F24";

		// All undefined or non working keys
	 	default: return "undefined";
	}
}

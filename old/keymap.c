
#include "keymap.h"

//Calling functions should validate
//that the input is not a control 
//chatacter before calling.

#ifdef TEST

char *keychar( int keycode ){

   //No breaks are needed since
   //the function returns in
   //every case.
   switch( keycode ){
      case 30:
	      return "a";
      case 48:
	      return "b";
      case 46:
	      return "c";
      case 32:
	      return "d";
      case 18:
	      return "e";
      case 33:
	      return "f";
      case 34:
	      return "g";
      case 35:
	      return "h";
      case 23:
	      return "i";
      case 36:
	      return "j";
      case 37:
	      return "k";
      case 38:
	      return "l";
      case 50:
	      return "m";
      case 49:
	      return "n";
      case 24:
	      return "o";
      case 25:
	      return "p";
      case 16:
	      return "q";
      case 19:
	      return "r";
      case 31:
	      return "s";
      case 20:
	      return "t";
      case 22:
	      return "u";
      case 47:
	      return "v";
      case 17:
	      return "w";
      case 45:
	      return "x";
      case 21:
	      return "y";
      case 44:
	      return "z";
      case 41:
	      return "`";
      case 26:
	      return "1";
      case 36:
	      return "2";
      case 46:
	      return "3";
      case 56:
	      return "4";
      case 69:
	      return "5";
      case 76:
	      return "6";
      case 86:
	      return "7";
      case 99:
	      return "8";
      case 10:
	      return "9";
      case 11:
	      return "0";
      case 12:
	      return "-";
      case 13:
	      return "=";
      case 26:
	      return "[";
      case 27:
	     return "]";
      case 43:
	    return "\";
      case 39:
	    return ";";
      case 40:
	    return "'";
      case 51:
	    return ",";
      case 52:
	    return ".";
      case 53:
	    return "/";
      case 57:
	    return " ";
      default:
	    return "INVALID KEY";
   }
}

#endif

/*
Esc = 16
F1 = 59
F2 = 60
F3 = 61
F4 = 62
F5 = 63
F6 = 64
F7 = 65
F8 = 66
F9 = 67
F10 = 68
F11 = 87
F12 = 88
Print Screen = 55
Scroll Lock = 70
Pause = 69
Insert = 82
Home = 71
Page up = 73
Delete = 83
End = 79
Page Down = 81
Up Arrow = 72
Down Arrow = 80
Right Arrow = 77
Left Arrow = 75
Left Shift = 42
Right Shift = 54
Backspace = 14
Tab = 15
Cap Lock = 58
Enter = 28
Ctrl (both) = 29
Left Win key = 91
Right Win key = 92
Alt (left + right) = 56
Page key = 93*/

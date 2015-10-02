#ifndef Led8x8xn_h
#define Led8x8xn_h

#include <Arduino.h>
#define NLEDS 2
class Led8x8xn{
	private: 
	byte m_DataPin[NLEDS],m_LoadPin,m_ClockPin;
	int m_strlen,m_direction,m_count,m_countStart,m_countEnd;
	byte *m_str;
	void getCharFont(byte Char,byte *mem);
	void putMax7219Byte(byte data); 
	void putMax7219Reg( byte reg, byte col);
  void copyStr(const char *str);
	public:
	byte Memory[NLEDS][8];
	bool Finish,CycleRotate,Mirror;
	Led8x8xn(byte DataPin[NLEDS],byte LoadPin,byte ClockPin);
	void setBrightness(byte Brightness); //0~15 
	void loadChar(byte Char,byte Idx);
	void scrollLeft(byte Char1,byte Char2,int n, byte Idx);
	void scrollRight(byte Char1,byte Char2,int n, byte Idx);
	void reflash();
	void stringScrollLeft(bool CycleRotate = false);
	void stringScrollRight(bool CycleRotate = false);	
	void stringScrollLeft(const char *str, bool CycleRotate = false);
	void stringScrollRight(const char *str, bool CycleRotate = false);
	void run();	
};
#endif

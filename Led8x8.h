#ifndef Led8x8_h
#define Led8x8_h

#include <Arduino.h>

class Led8x8{
	private: 
	int m_DataPin,m_LoadPin,m_ClockPin,m_strlen;
	int m_direction,m_count,m_countStart,m_countEnd;
	byte *m_str;
	void getCharFont(byte Char,byte *mem);
	void putMax7219Byte(byte data); 
	void putMax7219Reg( byte reg, byte col);
  void copyStr(const char *str);
	public:
	byte Memory[8];
	bool Finish,CycleRotate;
	Led8x8(int DataPin,int LoadPin,int ClockPin);
	void loadChar(byte Char);
	void scrollUp(byte Char1,byte Char2,int n);
	void scrollDown(byte Char1,byte Char2,int n);
	void scrollLeft(byte Char1,byte Char2,int n);
	void scrollRight(byte Char1,byte Char2,int n);
	void reflash();
	void stringScrollUp(bool CycleRotate = false);
	void stringScrollDown(bool CycleRotate = false);
	void stringScrollLeft(bool CycleRotate = false);
	void stringScrollRight(bool CycleRotate = false);	
	void stringScrollUp(const char *str, bool CycleRotate = false);
	void stringScrollDown(const char *str, bool CycleRotate = false);
	void stringScrollLeft(const char *str, bool CycleRotate = false);
	void stringScrollRight(const char *str, bool CycleRotate = false);
	void run();	
};
#endif

#include <avr/pgmspace.h>
#include "Led8x8.h"
#include "font8x8_basic.h"
#include "font8x8_control.h"
#include "font8x8_ext_latin.h"

#define maxReg maxBitmapLine
// define max7219 registers
#define max7219_reg_noop        0x00
#define max7219_reg_digit0      0x01
#define max7219_reg_digit1      0x02
#define max7219_reg_digit2      0x03
#define max7219_reg_digit3      0x04
#define max7219_reg_digit4      0x05
#define max7219_reg_digit5      0x06
#define max7219_reg_digit6      0x07
#define max7219_reg_digit7      0x08
#define max7219_reg_decodeMode  0x09
#define max7219_reg_intensity   0x0a
#define max7219_reg_scanLimit   0x0b
#define max7219_reg_shutdown    0x0c
#define max7219_reg_displayTest 0x0f


Led8x8::Led8x8(int DataPin,int LoadPin,int ClockPin)
	:m_DataPin(DataPin),m_LoadPin(LoadPin),m_ClockPin(ClockPin)
{ 
	pinMode(DataPin, OUTPUT);
  pinMode(LoadPin, OUTPUT);
  pinMode(ClockPin, OUTPUT);
	memset(Memory,0,8);	
	m_str = 0;
	m_direction = 0;
	m_count = 0;
	m_countStart = 0;
	m_countEnd = 0;
	Finish = false;
	CycleRotate = false;
	
	  //initiation of the max 7219
	  
  putMax7219Reg(max7219_reg_scanLimit, 0x07);
  putMax7219Reg(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  putMax7219Reg(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  putMax7219Reg(max7219_reg_displayTest, 0x00); // no display test
  for (int e = max7219_reg_digit0; e <= 8; e++) 
  { // empty registers, turn all LEDs off
    putMax7219Reg(e, 0);
  }
  putMax7219Reg(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
  // range: 0x00 to 0x0f
}
void Led8x8::getCharFont(byte Char,byte *mem)
{
	int i;
	if(Char<128)
	{
			for(i=0;i<8;i++)
			{
				mem[i]=pgm_read_byte_near(font8x8_basic[Char]+i);
			}
	}
	else
	{
			Char-=128;
		 	if(Char<32)
			{
				for(i=0;i<8;i++)
				{
					mem[i]=pgm_read_byte_near(font8x8_control[Char]+i);
				}
			}
			else
			{	
				Char-=32;
				for(i=0;i<8;i++)
				{
					mem[i]=pgm_read_byte_near(font8x8_ext_latin[Char]+i);
				}
			}
	 }
}
void Led8x8::loadChar(byte Char)
{
	getCharFont(Char,Memory);
}
#define RangeN() byte font1[8];	byte font2[8]; getCharFont(Char1,font1);	getCharFont(Char2,font2); if(n<1) {loadChar(Char1);return;} if(n>7) {loadChar(Char2);return;}

void Led8x8::scrollUp(byte Char1,byte Char2,int n)
{	
	RangeN();
	memcpy(Memory,font1+n,8-n);
	memcpy(Memory+(8-n),font2,n);	
}

void Led8x8::scrollDown(byte Char1,byte Char2,int n)
{
	RangeN();
	memcpy(Memory+n,font1,8-n);
	memcpy(Memory,font2+(8-n),n);
}

void Led8x8::scrollLeft(byte Char1,byte Char2,int n)
{
	int i;
	RangeN();
	for(i=0;i<8;i++)
		Memory[i]=(font1[i]>>n)|(font2[i]<<(8-n));
}
void Led8x8::scrollRight(byte Char1,byte Char2,int n)
{
	int i;
	RangeN();
	for(i=0;i<8;i++)
			Memory[i]=(font1[i]<<n)|(font2[i]>>(8-n));
}
void Led8x8::putMax7219Byte(byte data) 
{
	for(int i=0;i<8;i++,data<<=1)
	{
    digitalWrite(m_ClockPin, LOW);   // tick
    digitalWrite(m_DataPin, (data & 0x80)?HIGH:LOW);// send 1
    digitalWrite(m_ClockPin, HIGH);   // tock
  }
}

void Led8x8::putMax7219Reg( byte reg, byte col) 
{
  //maxSingle is the "easy"  function to use for a single max7219

  digitalWrite(m_LoadPin, LOW);       // begin
  putMax7219Byte(reg);                  // specify register
  putMax7219Byte(col);//((data & 0x01) * 256) + data >> 1); // put data
  digitalWrite(m_LoadPin, LOW);       // and load da stuff
  digitalWrite(m_LoadPin, HIGH);
}

void Led8x8::reflash()
{
	for (int e = max7219_reg_digit0,i=0; e <= 8; e++,i++) 
	{
  	digitalWrite(m_LoadPin, LOW);       // begin
  	putMax7219Byte(e);  
  	byte data=Memory[i];
 		for(int j=0;j<8;j++,data>>=1)
		{
    	digitalWrite(m_ClockPin, LOW);   // tick
    	digitalWrite(m_DataPin, (data & 0x1)?HIGH:LOW);// send 1
    	digitalWrite(m_ClockPin, HIGH);   // tock
  	}
  	digitalWrite(m_LoadPin, LOW);       // and load da stuff
  	digitalWrite(m_LoadPin, HIGH);
  }

}
void Led8x8::copyStr(const char *str)
{
	m_strlen=strlen(str);
	if(m_str)
		free(m_str);
	m_str = (byte*) malloc(m_strlen);
	
	strcpy((char*)m_str,str);
}
void Led8x8::stringScrollUp(bool CycleRotate)
{
	m_direction = 1;
	m_countStart = 0;
	m_count = m_countStart;
	Finish = false;
	if(CycleRotate)
		m_countEnd=8*m_strlen;
	else	
		m_countEnd=8*m_strlen-8;
}
void Led8x8::stringScrollUp(const char *str,bool CycleRotate)
{
	copyStr(str);
	stringScrollUp(CycleRotate);
}
void Led8x8::stringScrollDown(bool CycleRotate)
{
	m_direction = 2;
	m_countStart = m_strlen*8-1;
	m_count = m_countStart;
	Finish = false;
	if(CycleRotate)
		m_countEnd=0;
	else	
		m_countEnd=8;
	
}
void Led8x8::stringScrollDown(const char *str,bool CycleRotate)
{
	copyStr(str);
	stringScrollDown(CycleRotate);
}
void Led8x8::stringScrollLeft(bool CycleRotate)
{
	m_direction = 3;
	m_countStart = 0;
	m_count = m_countStart;
	Finish = false;
	if(CycleRotate)
		m_countEnd=8*m_strlen;
	else	
		m_countEnd=8*m_strlen-8;
}
void Led8x8::stringScrollLeft(const char *str,bool CycleRotate)
{
	copyStr(str);
	stringScrollLeft(CycleRotate);
}
void Led8x8::stringScrollRight(bool CycleRotate)
{
	m_direction = 4;
	m_countStart = m_strlen*8-1;
	m_count = m_countStart;
	Finish = false;
		if(CycleRotate)
		m_countEnd=0;
	else	
		m_countEnd=8;
}
void Led8x8::stringScrollRight(const char *str,bool CycleRotate)
{
	copyStr(str);
	stringScrollRight(CycleRotate);
}
void Led8x8::run()
{
	int cc=m_count>>3,rn=m_count&0x7,cn;


	switch(m_direction)
	{
		case 1:
					cn=cc+1;
					if(cn>=m_strlen)
						cn=0;
					scrollUp(m_str[cc],m_str[cn],rn);
					m_count++;
					if(m_count>=m_countEnd)
					{
						m_count=m_countStart;			
						Finish = true;
					}									

					reflash();
					break;
		case 2:
					cn=cc-1;
					if(cn<0)
						cn=m_strlen-1;			
					scrollDown(m_str[cc],m_str[cn],8-rn);
					m_count--;
					if(m_count<m_countEnd)
					{
						m_count = m_countStart;					
						Finish = true;
					}				
					reflash();
					break;
		case 3:
					cn=cc+1;
					if(cn>=m_strlen)
						cn=0;			
					scrollLeft(m_str[cc],m_str[cn],rn);
					m_count++;
					if(m_count>=m_countEnd)
					{
						m_count=m_countStart;			
						Finish = true;
					}								
					reflash();
					break;
		case 4:
					cn=cc-1;
					if(cn<0)
						cn=m_strlen-1;
					scrollRight(m_str[cc],m_str[cn],8-rn);
					m_count--;
					if(m_count<m_countEnd)
					{
						m_count = m_countStart;					
						Finish = true;
					}	
					reflash();
					break;
			
	}
}	
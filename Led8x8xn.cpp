#include <avr/pgmspace.h>
#include "Led8x8xn.h"
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


Led8x8xn::Led8x8xn(byte DataPin[],byte LoadPin,byte ClockPin)
	:m_LoadPin(LoadPin),m_ClockPin(ClockPin)
{ 
	byte i;
	memcpy(m_DataPin,DataPin,sizeof(byte)*NLEDS);
	for(i=0;i<NLEDS;i++)
		pinMode(m_DataPin[i], OUTPUT);
  pinMode(LoadPin, OUTPUT);
  pinMode(ClockPin, OUTPUT);
	memset(Memory,0,8*NLEDS);	
	m_str = 0;
	m_direction = 0;
	m_count = 0;
	m_countStart = 0;
	m_countEnd = 0;
	Finish = false;
	CycleRotate = false;
	Mirror = false;
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

void Led8x8xn::getCharFont(byte Char,byte *mem)
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

void Led8x8xn::loadChar(byte Char,byte Idx)
{
	getCharFont(Char,Memory[Idx]);
}

#define RangeN(void) \
{ \
	if(n<1) \
	{ \
		getCharFont(Char1 ,Memory[Idx]); \
		return; \
	} \
	else if(n>7) \
	{ \
		getCharFont(Char2 ,Memory[Idx]); \
		return; \
	} \
	else \
	{ \
		getCharFont(Char1,font1);	\
		getCharFont(Char2,font2); \
	} \
} 


void Led8x8xn::scrollLeft(byte Char1,byte Char2,int n, byte Idx)
{
	byte i;
	byte font1[8],font2[8];

	RangeN();
	for(i=0;i<8;i++)
		Memory[Idx][i]=(font1[i]>>n)|(font2[i]<<(8-n));
}
void Led8x8xn::scrollRight(byte Char1,byte Char2,int n, byte Idx)
{
	byte i;
	byte font1[8],font2[8];
	RangeN();
	for(i=0;i<8;i++)
			Memory[Idx][i]=(font1[i]<<n)|(font2[i]>>(8-n));
}

void Led8x8xn::putMax7219Byte(byte data) 
{
	for(byte i=0;i<8;i++,data<<=1)
	{
    digitalWrite(m_ClockPin, LOW);   // tick
    for(byte j=0;j<NLEDS;j++)
    	digitalWrite(m_DataPin[j], (data & 0x80)?HIGH:LOW);// send 1
    digitalWrite(m_ClockPin, HIGH);   // tock
  }
}

void Led8x8xn::putMax7219Reg( byte reg, byte col) 
{
  //maxSingle is the "easy"  function to use for a single max7219

  digitalWrite(m_LoadPin, LOW);       // begin
  putMax7219Byte(reg);                  // specify register
  putMax7219Byte(col);//((data & 0x01) * 256) + data >> 1); // put data
  digitalWrite(m_LoadPin, LOW);       // and load da stuff
  digitalWrite(m_LoadPin, HIGH);
}

void Led8x8xn::reflash()
{
	byte i,j,k;
	for(i=0; i < 8; i++) 
	{
		byte data[NLEDS];
		
		for(k=0;k<NLEDS;k++)
		{
		//	data[k] = Memory[k][i];
		
			data[k] = 0;
			for(j=0;j<8;j++)
			{
 		   data[k]<<=1;
 		   data[k] |= (Memory[k][j]>>i)&0x1;
 		  }
 		
 		}
 		   
  	digitalWrite(m_LoadPin, LOW);       // begin
  	putMax7219Byte(max7219_reg_digit0+i);  

  	if(!Mirror)
  	{
 			for(j=0;j<8;j++)
			{
    			digitalWrite(m_ClockPin, LOW);   // tick
    			for(k=0;k<NLEDS;k++)
    			{
    				digitalWrite(m_DataPin[k], (data[k] & 0x1)?HIGH:LOW);// send 1
    				data[k]>>=1;
    			}
    			digitalWrite(m_ClockPin, HIGH);   // tock
  		}
  	}
  	else
  	{
 			for(j=0;j<8;j++)
			{
    			digitalWrite(m_ClockPin, LOW);   // tick
    			for(k=0;k<NLEDS;k++)
    			{
    				digitalWrite(m_DataPin[k], (data[k] & 0x80)?HIGH:LOW);// send 1
    				data[k]<<=1;
    			}
    			digitalWrite(m_ClockPin, HIGH);   // tock
  		}
  	}
  	digitalWrite(m_LoadPin, LOW);       // and load da stuff
  	digitalWrite(m_LoadPin, HIGH);
  }

}

void Led8x8xn::copyStr(const char *str)
{
	m_strlen=strlen(str);
	if(m_str)
		free(m_str);
	m_str = (byte*) malloc(m_strlen);
	
	strcpy((char*)m_str,str);
}

void Led8x8xn::stringScrollLeft(bool CycleRotate)
{
	m_direction = 1;
	m_countStart = 0;
	m_count = m_countStart;
	Finish = false;
	if(CycleRotate)
		m_countEnd=8*m_strlen;
	else	
		m_countEnd=8*(m_strlen-NLEDS);
}

void Led8x8xn::stringScrollLeft(const char *str,bool CycleRotate)
{
	copyStr(str);
	stringScrollLeft(CycleRotate);
}

void Led8x8xn::stringScrollRight(bool CycleRotate)
{
	m_direction = 2;
	m_countStart = m_strlen*8-1;
	m_count = m_countStart;
	Finish = false;
		if(CycleRotate)
		m_countEnd=0;
	else	
		m_countEnd=NLEDS*8;
}

void Led8x8xn::stringScrollRight(const char *str,bool CycleRotate)
{
	copyStr(str);
	stringScrollRight(CycleRotate);
}

void Led8x8xn::run()
{
	int cc=m_count>>3,rn=m_count&0x7,cn1,cn2;


	switch(m_direction)
	{
		case 1:
			    cn2=cn1=cc;
					for(byte k=0;k<NLEDS;k++)	
					{
 					  cn2++;
					  if(cn2>=m_strlen)
						  cn2=0;
						scrollLeft(m_str[cn1],m_str[cn2],rn,k);
						cn1++;
						if(cn1>=m_strlen)
						  cn1=0;
			   	}
					m_count++;
					if(m_count>=m_countEnd)
					{
						m_count=m_countStart;			
						Finish = true;
					}									

					reflash();
					break;
		case 2:
					cn2=cn1=cc;
					for(char k=NLEDS-1;k>=0;k--)	
					{
 					  cn2--;
					  if(cn2<0)
						  cn2=m_strlen-1;			
					  scrollRight(m_str[cn1],m_str[cn2],8-rn,k);
 					  cn1--;
					  if(cn1<0)
						  cn1=m_strlen-1;								  
				  }
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
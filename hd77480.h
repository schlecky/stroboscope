#ifndef __hd77480_h__
#define __hd77480_h__

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00


#define SEND_CMD                   0
#define SEND_CHR                   1

#define LCD_PORT  P1OUT
#define LCD_DIR   P1DIR

#define RS    BIT0
#define E     BIT1 
#define D4  	BIT4	
#define D5    BIT5
#define D6    BIT6
#define D7    BIT7

static void __inline__ Delay(register unsigned int n)
{
    __asm__ __volatile__ (
		"1: \n"
		" dec	%[n] \n nop \n nop \n nop  \n nop \n nop \n nop \n"
		" jne	1b \n"
        : [n] "+r"(n));
}

void LCDPulseEnable(){
  LCD_PORT &= ~E;
  LCD_PORT |= E;
  LCD_PORT &= ~E;
}


void LCDWrite4Bits(unsigned char value)
{
  if(value & 0x01) LCD_PORT |= D4; else LCD_PORT&=~D4;
  if(value & 0x02) LCD_PORT |= D5; else LCD_PORT&=~D5; 
  if(value & 0x04) LCD_PORT |= D6; else LCD_PORT&=~D6; 
  if(value & 0x08) LCD_PORT |= D7; else LCD_PORT&=~D7; 
  
  LCDPulseEnable();
  Delay(100);
}

void LCDSend(unsigned char value, unsigned char mode) {
    if (mode==SEND_CHR) LCD_PORT |= RS; else LCD_PORT &= ~RS;
    LCDWrite4Bits(value>>4);
    LCDWrite4Bits(value);
}

//Efface la totalité de l'écran
void LCDClear(void) {
   LCDSend(LCD_CLEARDISPLAY,SEND_CMD);
   Delay(300);
}

// Va a une position prédéfinie
void LCDGotoXY(unsigned char x, unsigned char y)
{
  unsigned char delta;
  switch(y){
    case 0:
      delta = 0x00;
      break;
    case 1:
      delta = 0x40;
      break;
    case 2:
      delta = 20;
      break;
    case 3:
      delta = 0x40+20;
      break;
    default:
      delta=0;
  }
  delta = delta+x;
  LCDSend(LCD_SETDDRAMADDR|delta,SEND_CMD);
}

void LCDWriteInt2(int num)
{
    LCDSend('0'+num/10,SEND_CHR);
    LCDSend('0'+num%10,SEND_CHR);
}

void LCDWriteString(const unsigned char* str)
{
  int i;
  for(i=0; str[i]!=0; i++)
  {
    LCDSend(str[i],SEND_CHR);
  }
}

void LCDWriteInt3(int num)
{
  LCDSend('0'+(num/100)%10,SEND_CHR);
  LCDSend('0'+(num/10)%10,SEND_CHR);
  LCDSend('0'+num%10,SEND_CHR);
}

void LCDWriteInt4(int num)
{
  LCDSend('0'+num/1000,SEND_CHR);
  LCDSend('0'+(num/100)%10,SEND_CHR);
  LCDSend('0'+(num/10)%10,SEND_CHR);
  LCDSend('0'+num%10,SEND_CHR);
}

void LCDInit(void)
{ 
  LCD_DIR |= (E | RS | D4 | D5 | D6 | D7);
  //LCD_PORT &= ~E;
  //LCDPulseEnable();
  // we start in 8bit mode, try to set 4 bit mode
  LCD_PORT &= ~RS;
  LCDWrite4Bits(0x03);
  //LCDPulseEnable();
  Delay(1000);
  
  LCDWrite4Bits(0x03);
  //LCDPulseEnable();
  Delay(500);
  
  LCDWrite4Bits(0x03);
  //LCDPulseEnable();
  Delay(500);
  
  LCDWrite4Bits(0x02);
  //LCDPulseEnable();
  Delay(500);
  
  LCDSend(0x28,SEND_CMD);
  Delay(100);
  LCDSend(0x08,SEND_CMD);
  Delay(100);
  LCDSend(0x01,SEND_CMD);
  Delay(100);  
  LCDSend(0x06,SEND_CMD);
  Delay(100); 
  LCDSend(0x0C,SEND_CMD);
  Delay(100); 

  LCDSend(LCD_FUNCTIONSET |LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS, SEND_CMD);
  Delay(100);
  LCDSend(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF, SEND_CMD);
  Delay(100);
  LCDSend(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT, SEND_CMD);
}

#endif

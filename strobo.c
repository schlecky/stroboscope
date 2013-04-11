#include <legacymsp430.h>
#include <msp430g2452.h>

#include "hd77480.h"
#include "utils.h"

#define CPU_FREQ 1000000
#define LED_PORT P1OUT
#define LED_PIN  BIT2
#define LED_DIR  P1DIR
#define LED_SEL  P1SEL

#define BTN_IN  P2IN
#define BTN_IE  P2IE
#define BTN_IES  P2IES
#define BTN_IFG  P2IFG
#define BTN_OUT P2OUT
#define BTN_REN P2REN
#define BTN_MOL1 BIT2
#define BTN_MOL2 BIT1
#define BTN_CLIC BIT0

#define CH_FREQ 0
#define CH_CYCL 1

#define DELAI_ON 100 
 
float frequence; // Frequence du stroboscope
long int periode;     // Periode
int toChange;
int cycl_denom;
char freqstr[6];
volatile int delai_clic;
volatile float delta_freq;

NAKED(_reset_vector__)
{
	/* place your startup code here */

	/* Make sure, the branch to main (or to your start
	   routine) is the last line in the function */
	__asm__ __volatile__("br #main"::);
}

void writeFreq()
{
  LCDGotoXY(0,1);
  doubleToStr(frequence, freqstr);
  LCDWriteString(freqstr);
  LCDWriteString(" Hz");
}

void updateFreq()
{
  periode = (long int)(CPU_FREQ/(frequence*8));
  //periode = 3000;
  TACCR0 = periode;
  if(TAR>TACCR0) TAR=TACCR0;
  TACCR1 = periode-periode/cycl_denom;
}

void writeCycl()
{
  LCDGotoXY(0,1);
  LCDWriteString("1/"); 
  LCDWriteInt2(cycl_denom);
  LCDWriteString("        "); 
}

void main (void)
{
  WDTCTL = WDTPW + WDTHOLD;        // Stop watchdog timer
  BCSCTL1 = CALBC1_1MHZ;           // run at 1Mhz
  DCOCTL = CALDCO_1MHZ;
  Delay(10000);
  
  //LED
  LED_DIR |= LED_PIN;
  LED_SEL |= LED_PIN;
  
  // bouton
  BTN_REN |= (BTN_MOL1|BTN_MOL2|BTN_CLIC);  // enable pull_ups
  BTN_OUT |= (BTN_MOL1|BTN_MOL2|BTN_CLIC);  // pull_up
  BTN_IES |= (BTN_MOL2|BTN_CLIC);  // high to low transition
  BTN_IE  |= (BTN_MOL2|BTN_CLIC);  // Enable interrupts

  WRITE_SR(GIE); 		            //Enable global interrupts
  
  TACTL = TASSEL_2 | ID_3 | MC_1;  //SMCLK, DIV by 8, count UP
  TACCTL1 |= OUTMOD_2;             // toggle/reset

  LCDInit();
  frequence=2;
  cycl_denom = 16;
  delta_freq=0.1;
  LCDGotoXY(0,0);
  LCDWriteString("Frequence :");
  toChange = CH_FREQ;
  updateFreq();
  writeFreq();
  delai_clic=200;
  while(1)
  {
    if(delai_clic<200) delai_clic++;
    Delay(200);
  }
}


// Port 2 interrupt service routine
interrupt (PORT2_VECTOR) Port_2(void)
{
  if(BTN_IFG & BTN_MOL2)
  {
    BTN_IFG &= ~BTN_MOL2;
    BTN_IE &= ~BTN_MOL2;
    int i;
    int count=0;
    for(i=0;i<5;i++)
    {
      if(BTN_IN & BTN_MOL2) count++;
    }
    if(count==0)
    {
      for(i=0;i<5;i++)
      {
        if(BTN_IN & BTN_MOL1) count++;
      }
      if(toChange == CH_FREQ)
      {
        if(BTN_IN & BTN_CLIC)
          delta_freq = 0.1;
        else
          delta_freq = 5;
        //
        if(count==0){
          if(frequence<1000-delta_freq) frequence+=delta_freq;}
        else if(count==5){
          if(frequence>2+delta_freq) frequence-=delta_freq;}
        updateFreq();
        writeFreq();
      }
      else if(toChange == CH_CYCL){
        if(count==0){
          if(cycl_denom<33) cycl_denom*=2;}
        else if(count==5){
          if(cycl_denom>3) cycl_denom/=2;}
        writeCycl();
        updateFreq(); 
      }
    }
    BTN_IE |= BTN_MOL2;
  }
  else if(BTN_IFG & BTN_CLIC)
  {
    BTN_IFG &= ~BTN_CLIC;
    BTN_IE &= ~BTN_CLIC;
    int i,count=0;
    for(i=0;i<40;i++)
    {
      if(BTN_IN & BTN_CLIC) count++;
    }
    if(count==0)
    {
      if(delai_clic<190)  //double clic
      {
        LCDGotoXY(0,0);
        if(toChange == CH_FREQ){
          LCDWriteString("R. Cyclique :");
          writeCycl();
          toChange = CH_CYCL;
        }
        else if(toChange == CH_CYCL){
          LCDWriteString("Frequence :     ");
          writeFreq();
          toChange = CH_FREQ;
        }
      }
      delai_clic=0;
    }
    BTN_IE |= BTN_CLIC;
  }
}

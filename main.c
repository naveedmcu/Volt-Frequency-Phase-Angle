#include <main.h>
int16 counter1, counter2, HZ1,  HZ2, j;
int16 ADC_VALUE1,ADC_VALUE2;
int16 V1,V2;
int16 phase_time;


int1 g_flag;
int1 start_flag;
int1 f_s_start;

#int_RTCC
void  RTCC_isr(void) 
{
 SET_TIMER0(3036);
 HZ1 = counter1;
 HZ2 = counter2;
 counter1=0;
 counter2=0;
}

#int_timer1
void time1_isr(void)
{
 SET_TIMER1(0);
 start_flag = 0;
 f_s_start  = 0;
 phase_time = 0;
}

#int_EXT
void  EXT_isr(void) 
{
 counter1++;
 if (!start_flag)
 {
  SET_TIMER1(0);
  start_flag = 1;
  f_s_start = 0;
 }
 else if(start_flag )
 {
  phase_time=get_TIMER1();
  start_flag = 0;
  if (!f_s_start)
  {
   phase_time = 0;
  }
 }
}

#int_EXT1
void  EXT1_isr(void) 
{
 counter2++;
 if (!start_flag)
 {
  SET_TIMER1(0);
  start_flag = 1;
  f_s_start = 1;
 }
 else if(start_flag)
 {
  phase_time=get_timer1();
  start_flag = 0;
  if (start_flag)
  {
   phase_time=0;
  }
 }
}

#define ENABLE_PIN D0
#define RS_PIN D1
#define Data4 D4
#define Data5 D5
#define Data6 D6
#define Data7 D7


#define HZ_FAULT_LED    PIN_E0
#define VOLT_FAULT_LED  PIN_E1
#define PHASE_FAULT_LED PIN_E2

#include <lcd.c>


void main()
{
   SETUP_ADC_PORTS(NO_ANALOGS);
   setup_adc_ports(AN0_AN1_AN3);
   SETUP_ADC(ADC_CLOCK_INTERNAL);
   
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_16);      //65.5 ms overflow
   SET_TIMER0(3036);  

   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);      //65.5 ms overflow
   SET_TIMER1(0);

   setup_timer_3(T3_DISABLED | T3_DIV_BY_2);
   SET_TIMER3(15536);


   enable_interrupts(INT_RTCC);
   enable_interrupts(INT_timer1);
//   enable_interrupts(INT_timer3);
   enable_interrupts(INT_EXT);
   enable_interrupts(INT_EXT1);
   enable_interrupts(GLOBAL);
   
   SET_ADC_CHANNEL(0);
   READ_ADC(ADC_START_AND_READ);
   SET_ADC_CHANNEL(1);
   READ_ADC(ADC_START_AND_READ);
   
   
   OUTPUT_LOW(VOLT_FAULT_LED);
   OUTPUT_LOW(PHASE_FAULT_LED);
   OUTPUT_LOW(HZ_FAULT_LED);
   
   
   
   ADC_VALUE1 = READ_ADC(ADC_START_AND_READ);
   ADC_VALUE2 = READ_ADC(ADC_START_AND_READ);
   
   set_tris_d(0);
   lcd_init();
   lcd_gotoxy(1,1);
   lcd_putc('0');
   
   /*
   printf(lcd_putc, "\fThis is the 1st line");
   printf(lcd_putc, "\nNext is the 2nd line");
   printf(lcd_putc, "\nThis is the 3rd line");
   printf(lcd_putc, "\nFinally the 4th line"); 
   
   DELAY_ms(100);
   
   */
   SET_ADC_CHANNEL(0);
   DELAY_ms(100);
   READ_ADC(ADC_START_AND_READ);

   SET_ADC_CHANNEL(1);
   DELAY_ms(100);   
   READ_ADC(ADC_START_AND_READ);


  while(TRUE)
  {
   SET_ADC_CHANNEL(0);
   DELAY_ms(100);
   ADC_VALUE1 = READ_ADC(ADC_START_AND_READ);
   
   SET_ADC_CHANNEL(1);
   DELAY_ms(100);
   ADC_VALUE2 = READ_ADC(ADC_START_AND_READ);
 
   V1 = ADC_VALUE1*0.3051;
   V2 = ADC_VALUE2*0.3051;

   lcd_gotoxy(1,1);
   printf(lcd_putc,"V1:%4LU",(int16) V1);
   lcd_gotoxy(9,1);
   printf(lcd_putc,"|V2:%4LU",(int16) V2);
   
   lcd_gotoxy(1,2);
   printf(lcd_putc,"F1:%2LU",HZ1);
   lcd_gotoxy(9,2);
   printf(lcd_putc,"|F2:%2LU",HZ2);
  
   lcd_gotoxy(17,1);
   printf(lcd_putc,"PHASE:%4lU",(int16)(phase_time*.018));
  
   if ((HZ1 - HZ2) !=0)
   {
    OUTPUT_HIGH(HZ_FAULT_LED);
   }
   else 
   {
    OUTPUT_LOW(HZ_FAULT_LED);
   }
   if ((V1 - V2) !=0)
   {
    OUTPUT_HIGH(VOLT_FAULT_LED);
   }
   else 
   {
    OUTPUT_LOW(VOLT_FAULT_LED);
   }   
  }
}


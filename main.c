
#include "iostm8l152c6.h"
#include "stdlib.h"

#define STRINGVECTOR(x) #x
#define ISR( a, b ) \
  _Pragma( STRINGVECTOR( vector = (b) ) ) \
  __interrupt void (a)( void )

    unsigned int Count_flag = 1;
    unsigned int T_Count = 0;
    unsigned int T_Flag = 0;
    unsigned int H_Target = 0;
    unsigned int Hn = 0;
    unsigned int H = 0;
    unsigned int Way = 0;
    unsigned int ADC_Array[9];
    
ISR( TIM2_OVF, TIM2_OVR_UIF_vector )
{
  
  if ( T_Count <= 1000 )  //62745 ������ ����.
  { 
    T_Count++;
  } 
  else
  { 
    T_Count = 0;
    T_Flag = 1;         //�������� ���� ������ �������.
  }
  TIM2_SR1_bit.UIF = 0; //��������� ���� ����������.
}

void Init( void )
{
  CLK_CKDIVR = 0x00;     //������������ ����������� 1 ( ������� 16 ��� )
  
#define Red     1
#define Blue    0
#define Green   2
  
  PB_DDR = (1<<Red)|(1<<Blue)|(1<<Green);       //������������ ������ �� �����
  PB_CR1 = (1<<Red)|(1<<Blue)|(1<<Green);       //������� Push-Pull
  PB_ODR &= ~(1<<Red)|(1<<Blue)|(1<<Green);     //������������ ������ � 0
  
  PD_DDR = (1<<0);
  PD_CR1 = (1<<0);
  PD_ODR &= ~(1<<0);
  
  //��������� ��������.
  CLK_PCKENR1_bit.PCKEN10 = 1;   //����� ������������ �� ������ TIM2
  CLK_PCKENR1_bit.PCKEN11 = 1;   //����� ������������ �� ������ TIM3.
  TIM2_CR1_bit.URS = 1; //���������� ������ �� ������������ ��������.
  //��������� ������ PB0.
  TIM2_CCMR1_bit.OC1M = 0x06; //������������ PWM �� PB0, �������� 1.
  TIM2_CCMR1_bit.OC1PE = 1; //������� �������, ��� ���������� ������ PWM.
  TIM2_CCER1_bit.CC1E = 1; //������� ����� PB0.
  //��������� ������ PB1.
  TIM3_CCMR1_bit.OC1M = 0x06; //������������ PWM �� PB1, �������� 1.
  TIM3_CCMR1_bit.OC1PE = 1; //������� �������, ��� ���������� ������ PWM.
  TIM3_CCER1_bit.CC1E = 1; //������� ����� PB1.
  //��������� ������ PB2.
  TIM2_CCMR2_bit.OC2M = 0x06; //������������ PWM �� PB2, �������� 1.
  TIM2_CCMR2_bit.OC2PE = 1; //������� �������, ��� ���������� ������ PWM.
  TIM2_CCER1_bit.CC2E = 1; //������� ����� PB2.
  //��������� ������ PD0.
  TIM3_CCMR2_bit.OC2M = 0x06; //������������ PWM �� PB1, �������� 1.
  TIM3_CCMR2_bit.OC2PE = 1; //������� �������, ��� ���������� ������ PWM.
  TIM3_CCER1_bit.CC2E = 1; //������� ����� PB1.
  TIM2_BKR_bit.MOE = 1; //�������� PWM ��� TIM2.
  TIM3_BKR_bit.MOE = 1; //�������� PWM ��� TIM3.
  TIM2_EGR_bit.UG = 1;  //������� UpdateEvent, ����� ������ TIM2 ������ ���������.
  TIM3_EGR_bit.UG = 1;  //������� UpdateEvent, ����� ������ TIM3 ������ ���������.
  TIM2_IER_bit.UIE = 1; //�������� ���������� �� ������������ ��� TIM2.
  TIM2_CR1_bit.CEN = 1; //�������� TIM2.
  TIM3_CR1_bit.CEN = 1; //�������� TIM3.
  TIM2_ARRH = (255) >> 8;       //������������ ������� �������� ��� TIM2.
  TIM2_ARRL = (255) & 0xFF;
  TIM3_ARRH = (255) >> 8;       //������������ ������� �������� ��� TIM3.
  TIM3_ARRL = (255) & 0xFF;
  
  //��������� ���.
  CLK_PCKENR2_bit.PCKEN20 = 1; //����� ������������ �� ADC1.
  ADC1_CR1_bit.ADON = 1; //���������  ���.
  ADC1_CR1_bit.RES = 0x00; //12 ���.
  ADC1_CR2_bit.SMTP1 = 0x03; //����� ������ - 24 �����.
  ADC1_CR1_bit.CONT = 1; //������ ������������ ������.
  ADC1_SQR2_bit.CHSEL_S23 = 1; //������� ����� 23 �� PE5.
  ADC1_SQR1_bit.CHSEL_S28 = 1; //����� ����.
  ADC1_SQR3_bit.CHSEL_S9 = 1; //����� 9 �� PD5.
  ADC1_TRIGR2_bit.TRIG23 = 1; //�������� �������.
  ADC1_TRIGR1_bit.VREFINTON = 1; //�������� �������.
  ADC1_TRIGR3_bit.TRIG9 = 1; //�������� �������.
  
  //��������� DMA.
  CLK_PCKENR2_bit.PCKEN24 = 1; //����� ������������ �� DMA.
  DMA1_C0NDTR = 9; //������������ ������ ������.
  DMA1_C0PARH = 0x5344 >> 8; //�������� ������� ���� ��� �
  DMA1_C0PARL = (unsigned int) 0x5344;
  DMA1_C0M0ARH = (unsigned int)((unsigned int)ADC_Array >> 8); //������ � ����.
  DMA1_C0M0ARL = (unsigned int)ADC_Array; //������ � ����.
  DMA1_C0SPR_bit.TSIZE = 1;
  DMA1_C0CR_bit.CIRC = 1;
  DMA1_C0CR_bit.MINCDEC = 1;
  DMA1_GCSR_bit.TO = 0x00;
  DMA1_C0CR_bit.EN = 1;
  DMA1_GCSR_bit.GEN = 1;
  
  //��������� UART.
  CLK_PCKENR1_bit.PCKEN15 = 1; //����� ������������ �� USART1.
  
  USART1_BRR1 = 0x68;  //��������� �������� - 9600.
  USART1_BRR2 = 0x03;
  USART1_CR2 = (MASK_USART1_CR2_TEN)|(MASK_USART1_CR2_REN); 
  
}

void PWM( char Color, int Brightness )  //��� ������ ������������ ������� ������ � PWM.
{
  asm( "sim" );
  switch (Color)
  {
  case Red: {
              TIM3_CCR1H = ( Brightness ) >> 8;   //������� �����.
              TIM3_CCR1L = ( Brightness ) & 0xFF;
              break;
            }
  case Green: {
              TIM2_CCR2H = ( Brightness ) >> 8;   //�������� �����.
              TIM2_CCR2L = ( Brightness ) & 0xFF;
              break;
              }
  case Blue: {
              TIM2_CCR1H = ( Brightness ) >> 8;   //����� �����.
              TIM2_CCR1L = ( Brightness ) & 0xFF;
              break;
             }
  default: ;
  }
  asm( "rim" );
}

void HSV2RGB( int Hue )
{
  switch ( (Hue - ( Hue % 255 )) / 255 )
      {
      case 0: {
                PWM( Red, ( 255 - ( Hue % 255 ) ) );
                PWM( Green, Hue);
                PWM( Blue, 0x00 );
                break;
              }
      case 1: {
                PWM( Red, 0x00 );
                PWM( Green, ( 255 - ( Hue % 255 ) ) );
                PWM( Blue,  ( Hue - 255 ) );
                break;
              }
      case 2: {
                PWM( Red, ( Hue - 510 ) );
                PWM( Green, 0x00 );
                PWM( Blue, ( 255 - ( Hue % 255 ) ) );
                break;
              }
      default: ;
      }
}

void main( void )
{
  Init();       //TODO: ��������� ������ ��������� ����� SEPIC'� � �����.
  
  ADC1_CR1_bit.START = 1; //������ ��������������.
  
  PWM( Red, 0x00);
  PWM( Green, 0x00);
  PWM( Blue, 0x00);
  
  TIM3_CCR2H = ( 125 ) >> 8;   //SEPIC.
  TIM3_CCR2L = ( 125 ) & 0xFF;
 
  asm( "rim" );         //�������� ����������.
  
  srand( ADC_Array[3] );
  
  H = rand() % 766;
  
  HSV2RGB( H );
  
  while (1)
  {    
    if ( T_Flag == 1 ) 
    {
      
      //while (!(USART1_SR_TXE));
      //USART1_DR = 0xAA;
      
      if ( H == H_Target ) 
      {
        //PWM( Red, 0xff);        //���������� �������.
        //PWM( Green, 0xff);
        //PWM( Blue, 0xff);
        
        srand(ADC_Array[0]);     //TODO: ������� ���������� ������ �� ������� ����� ���.
        Hn = 85 + rand() % 596 ; //���� ��������� ���.
        if ( H_Target + Hn < 766 )
         {
           H_Target = H_Target + Hn;
         } 
        else 
         { 
          H_Target = H_Target + Hn - 765; 
         }
        srand(ADC_Array[6]);
        Way = rand() % 2;
      }
      else
      {
      if ( Way == 0 )
       {
         if ( H != 765 ) { H++; } else { H = 0; }   
       }
      else
       {
         if ( H != 0 ) { H--; } else { H = 765;  }
       }
      
      HSV2RGB( H );
      }
      
      T_Flag = 0;
    }
    
  }
}

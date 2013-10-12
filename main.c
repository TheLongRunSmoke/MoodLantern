
#include "iostm8l152c6.h"

#define STRINGVECTOR(x) #x
#define ISR( a, b ) \
  _Pragma( STRINGVECTOR( vector = (b) ) ) \
  __interrupt void (a)( void )

    unsigned int PWM_flag = 1;
    unsigned int PWM_R = 0x80;
    unsigned int PWM_G = 0x00;
    unsigned int PWM_B = 0x00;
    unsigned int PWM_Count = 0;
    
ISR(TIM2_OVF, TIM2_OVR_UIF_vector)
{
  
  PWM_flag = 1;         //�������� ���� PWM.
  
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
  
  CLK_PCKENR1_bit.PCKEN10 = 1;   //����� ������������ �� ������.
  TIM2_CR1_bit.URS = 1; //���������� ������ �� ������������ ��������.
  TIM2_EGR_bit.UG = 1;  //������� UpdateEvent, ����� ������ ������ ���������.
  TIM2_IER_bit.UIE = 1; //�������� ���������� �� ������������.
  TIM2_CR1_bit.CEN = 1; //�������� ������.
  
  TIM2_ARRH = (255) >> 8;       //������������ ������� �������� ��� �������.
  TIM2_ARRL = (255) & 0xFF;
  
}

void main( void )
{
  Init();
  
  asm( "rim" );         //�������� ����������.
  
  while (1)
  {
  if (PWM_flag == 1)
  {
    PWM_flag = 0;
    if ( PWM_R > PWM_Count ) {PB_ODR |= (1<<Red);} else {PB_ODR &= ~(1<<Red);}
    if ( PWM_G > PWM_Count ) {PB_ODR |= (1<<Green);} else {PB_ODR &= ~(1<<Green);}
    if ( PWM_B > PWM_Count ) {PB_ODR |= (1<<Blue);} else {PB_ODR &= ~(1<<Blue);}
    
    if ( PWM_Count < 0xFF) {PWM_Count++;} else {PWM_Count = 0;}
  }
  }
}

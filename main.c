
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
  
  PWM_flag = 1;         //Поднимаю флаг PWM.
  
  TIM2_SR1_bit.UIF = 0; //Сбрасываю флаг прерывания.
}


void Init( void )
{
  CLK_CKDIVR = 0x00;     //Устанавливаю пределитель 1 ( рабочая 16 Мгц )
  
#define Red     1
#define Blue    0
#define Green   2
  
  PB_DDR = (1<<Red)|(1<<Blue)|(1<<Green);       //Устанавливаю выводы на выход
  PB_CR1 = (1<<Red)|(1<<Blue)|(1<<Green);       //Включаю Push-Pull
  PB_ODR &= ~(1<<Red)|(1<<Blue)|(1<<Green);     //Устанавливаю выходы в 0
  
  CLK_PCKENR1_bit.PCKEN10 = 1;   //Подаю тактирование на таймер.
  TIM2_CR1_bit.URS = 1; //Прерывания только по переполнению счетчика.
  TIM2_EGR_bit.UG = 1;  //Вызываю UpdateEvent, чтобы таймер принял установки.
  TIM2_IER_bit.UIE = 1; //Разрешаю прерывания по переполнению.
  TIM2_CR1_bit.CEN = 1; //Запускаю таймер.
  
  TIM2_ARRH = (255) >> 8;       //Устанавливаю верхнее значение для таймера.
  TIM2_ARRL = (255) & 0xFF;
  
}

void main( void )
{
  Init();
  
  asm( "rim" );         //Разрешаю прерывания.
  
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

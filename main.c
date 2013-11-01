
#include "iostm8l152c6.h"

#define STRINGVECTOR(x) #x
#define ISR( a, b ) \
  _Pragma( STRINGVECTOR( vector = (b) ) ) \
  __interrupt void (a)( void )

    unsigned int Count_flag = 1;
    unsigned int PWM_R = 0xff;
    unsigned int PWM_G = 0x80;
    unsigned int PWM_B = 0x10;
    unsigned int T_Count = 0;
    unsigned int T_Flag = 0;
    
ISR(TIM2_OVF, TIM2_OVR_UIF_vector)
{
  
  if ( T_Count <= 62745 ) 
  { 
    T_Count++;
  } 
  else
  { 
    T_Count = 0;
    T_Flag = 1;         //Поднимаю флаг осчета времени.
  }
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
  
  CLK_PCKENR1_bit.PCKEN10 = 1;   //Подаю тактирование на таймер TIM2
  CLK_PCKENR1_bit.PCKEN11 = 1;   //Подаю тактирование на таймер TIM3.
  TIM2_CR1_bit.URS = 1; //Прерывания только по переполнению счетчика.
  //Настройка вывода PB0.
  TIM2_CCMR1_bit.OC1M = 0x06; //Устанавливаю PWM на PB0, активная 1.
  TIM2_CCMR1_bit.OC1PE = 1; //Включаю прелоад, для правильной работы PWM.
  TIM2_CCER1_bit.CC1E = 1; //Включаю вывод PB0.
  //Настройка вывода PB1.
  TIM3_CCMR1_bit.OC1M = 0x06; //Устанавливаю PWM на PB1, активная 1.
  TIM3_CCMR1_bit.OC1PE = 1; //Включаю прелоад, для правильной работы PWM.
  TIM3_CCER1_bit.CC1E = 1; //Включаю вывод PB1.
  //Настройка вывода PB2.
  TIM2_CCMR2_bit.OC2M = 0x06; //Устанавливаю PWM на PB2, активная 1.
  TIM2_CCMR2_bit.OC2PE = 1; //Включаю прелоад, для правильной работы PWM.
  TIM2_CCER1_bit.CC2E = 1; //Включаю вывод PB2.
  TIM2_BKR_bit.MOE = 1; //Разрешаю PWM для TIM2.
  TIM3_BKR_bit.MOE = 1; //Разрешаю PWM для TIM3.
  TIM2_EGR_bit.UG = 1;  //Вызываю UpdateEvent, чтобы таймер TIM2 принял установки.
  TIM3_EGR_bit.UG = 1;  //Вызываю UpdateEvent, чтобы таймер TIM3 принял установки.
  TIM2_IER_bit.UIE = 1; //Разрешаю прерывания по переполнению для TIM2.
  TIM2_CR1_bit.CEN = 1; //Запускаю TIM2.
  TIM3_CR1_bit.CEN = 1; //Запускаю TIM3.
  
  TIM2_ARRH = (255) >> 8;       //Устанавливаю верхнее значение для TIM2.
  TIM2_ARRL = (255) & 0xFF;
  
  TIM3_ARRH = (255) >> 8;       //Устанавливаю верхнее значение для TIM3.
  TIM3_ARRL = (255) & 0xFF;
  
}

void PWM( char Color, int Brightness )  //Эта фукция обеспечивает удобный доступ к PWM.
{
  asm( "sim" );
  switch (Color)
  {
  case Red: {
              TIM3_CCR1H = ( Brightness ) >> 8;   //Красный канал.
              TIM3_CCR1L = ( Brightness ) & 0xFF;
            }
  case Green: {
              TIM2_CCR2H = ( Brightness ) >> 8;   //Зеленный канал.
              TIM2_CCR2L = ( Brightness ) & 0xFF;
            }
  case Blue: {
              TIM2_CCR1H = ( Brightness ) >> 8;   //Синий канал.
              TIM2_CCR1L = ( Brightness ) & 0xFF;
            }
  default: ;
  }
  asm( "rim" );
}

void main( void )
{
  Init();
  
  //asm( "rim" );         //Разрешаю прерывания.
  
  /*TIM2_CCR1H = ( PWM_B ) >> 8;       //Устанавливаю значение для PWM.
  TIM2_CCR1L = ( PWM_B ) & 0xFF;
  
  TIM2_CCR2H = ( PWM_G ) >> 8;       //Устанавливаю значение для PWM.
  TIM2_CCR2L = ( PWM_G ) & 0xFF;
  
  TIM3_CCR1H = ( PWM_R ) >> 8;       //Устанавливаю значение для PWM.
  TIM3_CCR1L = ( PWM_R ) & 0xFF;*/
  
  PWM( Red, 0x80 );
  PWM( Green, 0xff );
  PWM( Blue, 0x10 );
  
  asm( "rim" );         //Разрешаю прерывания.
  
  while (1)
  {
    if ( T_Flag == 1 ) { PWM_B = ~PWM_B; T_Flag = 0; }
    
    PWM( Blue, PWM_B );
    
  }
}

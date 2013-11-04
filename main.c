
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
    
ISR(TIM2_OVF, TIM2_OVR_UIF_vector)
{
  
  if ( T_Count <= 1000 )  //62745 должно быть.
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
  
  //Настройка таймеров.
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
              break;
            }
  case Green: {
              TIM2_CCR2H = ( Brightness ) >> 8;   //Зеленный канал.
              TIM2_CCR2L = ( Brightness ) & 0xFF;
              break;
              }
  case Blue: {
              TIM2_CCR1H = ( Brightness ) >> 8;   //Синий канал.
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
  Init();       //TODO: Придумать способ отследить выход SEPIC'а в режим.
  PWM( Red, 0x00);
  PWM( Green, 0x00);
  PWM( Blue, 0x00);
  
  asm( "rim" );         //Разрешаю прерывания.
  
  srand( NULL );
  
  H = rand() % 766;
  
  HSV2RGB( H );
  
  while (1)
  {
    if ( T_Flag == 1 ) 
    {
            
      if ( H == H_Target ) 
      {
        //PWM( Red, 0xff);        //Отладочная вспышка.
        //PWM( Green, 0xff);
        //PWM( Blue, 0xff);
        
        srand(H);     //TODO: сделать скидывание мусора из младших битов АЦП.
        Hn = 85 + rand() % 596 ; //Беру случайный тон.
        if ( H_Target + Hn < 766 )
         {
           H_Target = H_Target + Hn;
         } 
        else 
        { 
          H_Target = H_Target + Hn - 765; 
        }
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

#include "gpio.h"
#include "stm32f303xc.h"

void
GPIO_Config(void) // Настройка портов
{
  RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);

  GPIOA->MODER |= (GPIO_MODER_MODER0_0 |
                   GPIO_MODER_MODER0_1); // PA0 и PA1 для аналогового входа
  GPIOA->MODER |= (GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1);

  GPIOA->MODER &= ~(GPIO_MODER_MODER2);
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0;

  GPIOE->MODER |=
    (GPIO_MODER_MODER8_0 /*| GPIO_MODER_MODER9_0*/ | GPIO_MODER_MODER10_0 |
     GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 |
     GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0); // Настройка портов на выход

  GPIOE->MODER &= ~(GPIO_MODER_MODER9);
  GPIOE->MODER |= GPIO_MODER_MODER9_1;
  GPIOE->AFR[1] |= (2 << GPIO_AFRH_AFRH1_Pos);
}

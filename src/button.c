#include "button.h"
#include "stm32f303xc.h"

volatile uint8_t led_state = 0;

void
Button_Config(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA;

  EXTI->IMR |= EXTI_IMR_IM2; // Включение линии прерывания EXTI2
  EXTI->RTSR |= EXTI_RTSR_TR2; // Прерывание по нарастающему фронту

  NVIC_SetPriority(EXTI2_TSC_IRQn, 0);
  NVIC_EnableIRQ(EXTI2_TSC_IRQn); // Включение прерывания в NVIC
}

void
EXTI2_TSC_IRQHandler(void)
{
  if (EXTI->PR & EXTI_PR_PR2) {
    EXTI->PR |= EXTI_PR_PR2; // Сброс флага прерывания

    for (volatile int i = 0; i < 20; i++)
      ;

    led_state = !led_state;
    if (led_state) {
      GPIOE->ODR |= GPIO_ODR_8; // Установить бит на PE8
    } else {
      GPIOE->ODR &= ~GPIO_ODR_8; // Сбросить бит на PE8
    }
  }
}

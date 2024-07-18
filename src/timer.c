#include "timer.h"
#include "stm32f303xc.h"
#include "variables.h"

void
Timer_Config(void) // Настройка таймера для генерации ШИМ
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // Включение тактирование таймеров

  TIM1->PSC = 72 - 1;  // Предделитель
  TIM1->ARR = 255 - 1; // Период

  TIM1->CCR1 = 140;                 // Установка уровня ШИМ
  TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); // Очистка полей OC1M
  TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // Установка режима ШИМ1
  TIM1->CCMR1 |=
    TIM_CCMR1_OC1PE; // Включение предварительной загрузки регистра сравнения
  TIM1->CCER |= TIM_CCER_CC1E; // Включение вывода сигнала ШИМ на
                               // соответствующий пин (в данном случае PE9)
  TIM1->BDTR |= TIM_BDTR_MOE; // Активация основного выхода таймер (необходимо
                              // для генерации ШИМ)
  TIM1->CR1 |= TIM_CR1_ARPE; // Включение автоперезагрузки

  TIM1->CR1 |= TIM_CR1_CEN; // Включение таймера
}

void
SysTick_Handler(void)
{
  uint8_t adc_conv = 0; // Номер массива для хранения значения
  ADC1->CR |= ADC_CR_ADSTART; // Начало преобразования АЦП
  for (adc_conv = 0; adc_conv < 2; adc_conv++) {
    while (!(ADC1->ISR & ADC_ISR_EOC))
      ; // Ждем окончания преобразования
    VR[adc_conv] = ADC1->DR; // Сохранение результата в массив
  }
  ADC1->ISR |= ADC_ISR_EOC; // Сброс флага
  // Проверка значений VR и установка соответствующих битов
  if (VR[1] > 2200) { // Ниже комбинации для поворота вниз
    if (state == 0) {
      GPIOE->ODR &= ~GPIO_ODR_10;
      GPIOE->ODR |= GPIO_ODR_11;
    } else if (state == 1) {
      GPIOE->ODR |= GPIO_ODR_10;
      GPIOE->ODR |= GPIO_ODR_11;
    } else if (state == 2) {
      GPIOE->ODR |= GPIO_ODR_10;
      GPIOE->ODR &= ~GPIO_ODR_11;
    } else {
      GPIOE->ODR &= ~(GPIO_ODR_10 | GPIO_ODR_11);
    }
  } else if (VR[1] < 1700) { // Ниже комбинация для поворота вверх
    if (state == 0) {
      GPIOE->ODR |= GPIO_ODR_10;
      GPIOE->ODR &= ~GPIO_ODR_11;
    } else if (state == 1) {
      GPIOE->ODR |= GPIO_ODR_10;
      GPIOE->ODR |= GPIO_ODR_11;
    } else if (state == 2) {
      GPIOE->ODR &= ~GPIO_ODR_10;
      GPIOE->ODR |= GPIO_ODR_11;
    } else {
      GPIOE->ODR &= ~(GPIO_ODR_10 | GPIO_ODR_11);
    }
  } else {
    GPIOE->ODR &= ~(GPIO_ODR_11 | GPIO_ODR_10);
  }
  state++; // Увеличение переменной для шага двигателя
  if (state >= 4)
    state = 0; // Сброс переменной, потому что всего 4 состояния может быть
}

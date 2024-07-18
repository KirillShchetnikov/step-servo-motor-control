#include "adc.h"
#include "stm32f303xc.h"
#include "variables.h"

void
ADC_Config(void) // Настройка АЦП
{
  RCC->AHBENR |= RCC_AHBENR_ADC12EN; // Включение тактирования для ADC1 и DMA1

  RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1; // Делим системное тактирование на 1
  ADC12_COMMON->CCR |= ADC_CCR_CKMODE_0; // Использование тактирование от AHB/2

  ADC1->SQR1 |= ADC_SQR1_L_0; // Установка количества каналов
  ADC1->SQR1 |=
    (1 << ADC_SQR1_SQ1_Pos) |
    (2 << ADC_SQR1_SQ2_Pos); // Установка порядка каналов для преобразований
  ADC1->SMPR1 |=
    (5 << ADC_SMPR1_SMP1_Pos) |
    (5 << ADC_SMPR1_SMP2_Pos); // Установка времени выборки АЦП (61.5 циклов)

  ADC1->CR &= ~ADC_CR_ADVREGEN_1; // Выключение регулятора напряжения
  ADC1->CR |= ADC_CR_ADVREGEN_0; // Включения регулятора напряжения

  // Калибровка АЦП
  ADC1->CR &= ~ADC_CR_ADCALDIF; // Калибровка в обычном режиме
  ADC1->CR |= ADC_CR_ADCAL;
  while (ADC1->CR & ADC_CR_ADCAL)
    ; // Ожидание конца калибровки

  ADC1->CR |= ADC_CR_ADEN; // Включение АЦП
  while (!(ADC1->ISR & ADC_ISR_ADRDY))
    ; // Ожидание готовности АЦП

  NVIC_EnableIRQ(ADC1_2_IRQn); // Включение прерываний от АЦП
}

void
ADC_Error_Handler(void) // Обработчик ошибки АЦП
{
  if (ADC1->ISR & ADC_ISR_ADRDY) // Проверка наличия ошибки
  {
    ADC1->CR |= ADC_CR_ADDIS; // Отключаем АЦП
    while (ADC1->CR & ADC_CR_ADEN)
      ; // Ждем, пока АЦП выключится

    VR[0] = 0; // Очистка массива
    VR[1] = 0;

    ADC1->CR |= ADC_CR_ADEN; // Включаем АЦП
    while (!(ADC1->ISR & ADC_ISR_ADRDY))
      ; // Ждем, пока АЦП будет готов

    ADC1->CR |= ADC_CR_ADSTART; // Начало преобразование
  }
}

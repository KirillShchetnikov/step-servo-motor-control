#include "systemclock.h"
#include "stm32f303xc.h"

void
SystemClock_Config(void) // Настройка тактирования
{
  // Включаем HSE (внешний кварцевый резонатор)
  RCC->CR |= (1 << RCC_CR_HSEON_Pos);
  while ((RCC->CR & RCC_CR_HSERDY) == 0) {
  }

  // Конфигурируем и запускаем PLL
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV; // Источник для PLL - HSE
  RCC->CFGR |= RCC_CFGR_PLLMUL5; // Умножаем частоту HSE на 5 (14,7456*5=73,728)
  RCC->CR |= RCC_CR_PLLON; // Включаем PLL
  while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
  } // Ждем пока PLL заработает

  // Конфигурируем Flash память
  FLASH->ACR |= FLASH_ACR_LATENCY_1; // 2 цикла задержки для Flash

  // Настраиваем делители частоты шин
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = SYSCLK / 1
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = HCLK / 2
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = HCLK / 1

  // Выбираем PLL как источник системной частоты
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
  }

  // Настраиваем тактирование ADC
  RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1; // ADC12 PLL clock divider set to 1

  RCC->CR &= ~(1 << RCC_CR_HSION_Pos); // Отключение HSI

  SysTick_Config(7200000 - 1); // Настройка системного таймера на 1 мс
  NVIC_SetPriority(SysTick_IRQn,
                   0); // Установка прерываний от системного таймера
}

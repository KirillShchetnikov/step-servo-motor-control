#include "adc.h"
#include "button.h"
#include "gpio.h"
#include "stm32f303xc.h"
#include "systemclock.h"
#include "timer.h"
#include "variables.h"

uint32_t VR[2] = { 0, 0 }; // Объявление массива для хранения данных с ацп
uint8_t state = 0;

int
main(void)
{
  SystemClock_Config();
  GPIO_Config();
  ADC_Config();
  Timer_Config();
  Button_Config();

  while (1) {
  }
}

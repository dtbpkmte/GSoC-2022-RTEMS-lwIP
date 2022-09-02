#include "stm32f4_lwip.h"

extern ETH_HandleTypeDef heth;

__attribute__((weak)) void Error_Handler(void) {
    __disable_irq();
    while (1)
    {
    }
}

void ETH_IRQHandler(void) {
    HAL_ETH_IRQHandler(&heth);
}

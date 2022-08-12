#include "stm32f4_lwip.h"

__attribute__((weak)) void Error_Handler(void) {
    __disable_irq();
    while (1)
    {
    }
}

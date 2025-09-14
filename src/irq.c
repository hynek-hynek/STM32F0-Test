#include "stm32f0xx.h"
#include "uart_stm32f0xx.h"

void USART1_IRQHandler(void) {
    // Zavola IRQ handler
    uart_irq_handler_stm32f0xx(0);
}

void USART2_IRQHandler(void) {
    // Zavola IRQ handler
    uart_irq_handler_stm32f0xx(1);
}

void DMA1_Channel2_3_IRQHandler(void)
{
    // Zavola IRQ handler
    uart_dma_irq_handler_stm32f0xx(0);
}

void DMA1_Channel4_5_IRQHandler(void)
{
    // Zavola IRQ handler
    uart_dma_irq_handler_stm32f0xx(1);
}

#include "stm32f103x6.h"

void UART_send(USART_TypeDef* USARTx, const char* ptrData) {
    while (*ptrData) {
        while (!(USARTx->SR & USART_SR_TXE));       // Чекаємо, доки буфер передачі буде готовий для відправлення наступного байту
        USARTx->DR = *ptrData++;                    // Записуємо дані для відправлення в регістр даних (DR)
    }
    
    while (!(USARTx->SR & USART_SR_TC));    // Чекаємо завершення передачі
}

extern "C" void TIM2_IRQHandler()
{
    UART_send(USART1, "Hello, World!\r\n");

    TIM2->SR &= ~TIM_SR_UIF;
}

int main()
{
        //SETTING TIMER
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->PSC = 8000 - 1;               // HCLK=8MHz, 8 MHz / (prescaler + 1) = 1000 Hz
    TIM2->ARR = 1000 - 1;               // 1000 Hz / (period + 1) = 1 Hz = 1 in 1s 

    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 = TIM_CR1_CEN;


        //SETTING UART
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->BRR = 8000000 / 9600;                   // Швидкість 9600 біт/с
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;     // Включення передачі та включення UART

    

    while (true)
    {
    }
    
    return 0;
}
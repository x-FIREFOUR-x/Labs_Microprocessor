#include "stm32f103x6.h"
#include <stdio.h>

void UART_send(USART_TypeDef* USARTx, const char* ptrData) {
    while (*ptrData) {
        while (!(USARTx->SR & USART_SR_TXE));       // Чекаємо, доки буфер передачі буде готовий для відправлення наступного байту
        USARTx->DR = *ptrData++;                    // Записуємо дані для відправлення в регістр даних (DR)
    }
    
    while (!(USARTx->SR & USART_SR_TC));    // Чекаємо завершення передачі
}

int main()
{
        //SETTING UART
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->BRR = 8000000 / 9600;                   // Швидкість 9600 біт/с
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;     // Включення передачі та включення UART

    
        //Setting ADC (АЦП)
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT;                                   // увімкнути АЦП, увімкнути режим перетворення
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2;      // встановлюємо час перевірки значення
    ADC1->CR2 |= ADC_CR2_SWSTART;                                               // запуск АЦП


    while (true)
    {
        while (!(ADC1->SR & ADC_SR_EOC));

        char value_adc[50];
        snprintf(value_adc, 50, "ADC = %d \r\n", ADC1->DR);
        UART_send(USART1, value_adc);

        ADC1->SR &= ~ADC_SR_EOC;
    }
    
    return 0;
}
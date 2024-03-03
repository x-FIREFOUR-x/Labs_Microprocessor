#include "stm32f103x6.h"
#include <stdio.h>
#include <string>


void UART_send(USART_TypeDef* USARTx, char ch) {
    while (!(USARTx->SR & USART_SR_TXE)){};       // Чекаємо, доки буфер передачі буде готовий для відправлення наступного байту
    USARTx->DR = ch;                            // Записуємо дані для відправлення в регістр даних (DR)
}

void UART_send(USART_TypeDef* USARTx, std::string message) {
    for (auto s : message) {
        UART_send(USARTx, s);
    }
    
    while (!(USARTx->SR & USART_SR_TC));    // Чекаємо завершення передачі
}

std::string cmd;
extern "C" void USART2_IRQHandler()
{
    if (USART2->DR != '\n' && USART2->DR != '\r')
    {
        cmd.push_back(USART2->DR);
        USART2->SR &= ~USART_SR_RXNE;
        return;
    }

    if (cmd == "on")
    {
        GPIOA->BSRR = GPIO_BSRR_BS1;
    }
    else if (cmd == "off")
    {
        GPIOA->BSRR = GPIO_BSRR_BR1;
    }

    cmd = "";
    USART2->SR &= ~USART_SR_RXNE;
}

int main()
{
        //SETTING UART
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->BRR = 8000000 / 9600;                   // Швидкість 9600 біт/с
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;     // увімкнення передачі та включення UART

    
        //SETTING ADC (АЦП)
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT;                                   // увімкнути АЦП, увімкнути режим перетворення
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2;      // встановлюємо час перевірки значення
    ADC1->CR2 |= ADC_CR2_SWSTART;                                               // запуск АЦП


        //SETTING UART for command
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    USART2->BRR = 8000000 / 9600;                                      // Швидкість 9600 біт/с
    USART2->CR1 |= USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;     // увімкнення отримання | увімкнення UART | увімкнення переривання
    NVIC_EnableIRQ(USART2_IRQn);

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= GPIO_CRL_MODE1_0;

    while (true)
    {
        while (!(ADC1->SR & ADC_SR_EOC)){};

        char value_adc[50];
        snprintf(value_adc, 50, "ADC = %d \r\n", ADC1->DR);
        UART_send(USART1, value_adc);

        ADC1->SR &= ~ADC_SR_EOC; 
    }
    
    return 0;
}
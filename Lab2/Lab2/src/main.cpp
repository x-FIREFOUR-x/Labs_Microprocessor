#include "stm32f103x6.h"
#include <stdio.h>
#include <string>

void UART_send(USART_TypeDef* USARTx, char ch) {
    while (!(USARTx->SR & USART_SR_TXE)){};
    USARTx->DR = ch;
}

void UART_send(USART_TypeDef* USARTx, std::string message) {
    for (auto s : message) 
        UART_send(USARTx, s);
    while (!(USARTx->SR & USART_SR_TC)) {};
}

void handle_cmd(std::string& line)
{
    if (line == "on")
    {
        GPIOA->BSRR = GPIO_BSRR_BS1;
    }
    else if (line == "off")
    {
        GPIOA->BSRR = GPIO_BSRR_BR1;
    }
    else if (line.substr(0, 6) == "timer:")
    {
        int intPart = atoi(&line.substr(6)[0]);
        float fractionalPart = atof(&line.substr(6)[0]);
        if (intPart >= 0 && fractionalPart >= 0 && (intPart + fractionalPart) > 0)
        {
            TIM2->CR1 &= ~TIM_CR1_CEN;
            TIM2->CNT = 0;
            TIM2->ARR = (1000 * (intPart + fractionalPart)) - 1;
            TIM2->CR1 |= TIM_CR1_CEN;
        } 
    }
    else if (line.substr(0, 6) == "pwd-p:")
    {
        if (line.substr(6,1) == "0")
        {
            TIM1->CR1 &= ~TIM_CR1_CEN;
            TIM1->CNT = 0;
            TIM1->CCER &= ~TIM_CCER_CC1P;
            TIM1->CR1 |= TIM_CR1_CEN;
        }
        else if (line.substr(6,1) == "1")
        {
            TIM1->CR1 &= ~TIM_CR1_CEN;
            TIM1->CNT = 0;
            TIM1->CCER |= TIM_CCER_CC1P;
            TIM1->CR1 |= TIM_CR1_CEN;
        }
        
    }
}

std::string cmd;
extern "C" void USART2_IRQHandler()
{
    if (USART2->DR != '\n' && USART2->DR != '\r')
    {
        cmd.push_back(USART2->DR);
    }
    else
    {
        handle_cmd(cmd);
        cmd = "";
    }
    USART2->SR &= ~USART_SR_RXNE;
}

extern "C" void TIM2_IRQHandler()
{
    GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR2) ? GPIO_BSRR_BR2 : GPIO_BSRR_BS2;
    TIM2->SR &= ~TIM_SR_UIF;
}

int main()
{
        // 1.1 SETTING UART
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    USART1->BRR = 8000000 / 9600;                   // Швидкість 9600 біт/с
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;     // увімкнення передачі та включення UART
    
        // 1.2 SETTING ADC (АЦП)
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT;                                   // увімкнути АЦП, увімкнути режим перетворення
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2;      // встановлюємо час перевірки значення
    ADC1->CR2 |= ADC_CR2_SWSTART;                                               // запуск АЦП

        // 2 SETTING UART for command on/off led
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    USART2->BRR = 8000000 / 9600;                                      // Швидкість 9600 біт/с
    USART2->CR1 |= USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;     // увімкнення отримання | увімкнення UART | увімкнення переривання
    NVIC_EnableIRQ(USART2_IRQn);

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= GPIO_CRL_MODE1_0;

        // 3 SETTING TIMER (and led) for command 
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->PSC = 8000 - 1;               // HCLK=8MHz, 8 MHz / (prescaler + 1) = 1000 Hz
    TIM2->ARR = 1000 - 1;       // 1000 Hz / (period + 1) => 0.8 Hz => 1 in 0.8s => 1.25 in 1s  (T1 = 2.3)
    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 = TIM_CR1_CEN;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= GPIO_CRL_MODE2_0;

        //3 SETTING ШІМ to PA8(base), PA7 for command 
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    GPIOA->CRH &= ~(GPIO_CRH_CNF8, GPIO_CRH_MODE8);
    GPIOA->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_CNF8_1;

    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_0;

    TIM1->PSC = 8000 - 1;
    TIM1->ARR = 1000 - 1;
    TIM1->CCR1 = 250 - 1;

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // on ШІМ
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P;
    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_CEN;

    while (true)
    {
            //1.3 Output ADC value to UART
        while (!(ADC1->SR & ADC_SR_EOC)){};

        char value_adc[50];
        snprintf(value_adc, 50, "ADC = %d \r\n", (int)(ADC1->DR));
        UART_send(USART1, value_adc);

        ADC1->SR &= ~ADC_SR_EOC;
    }
    
    return 0;
}
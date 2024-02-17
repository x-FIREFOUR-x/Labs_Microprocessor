#include "stm32f103x6.h"

    // N = 23 = 23 - 15 = 8;  T1 = N / 10 = 0.8

extern "C" void EXTI15_10_IRQHandler()
{
    if(GPIOA->IDR & GPIO_IDR_IDR10)
    {
            //turn on led 1-5
        GPIOA->BSRR = GPIO_BSRR_BS0;
        GPIOA->BSRR = GPIO_BSRR_BS1;
        GPIOA->BSRR = GPIO_BSRR_BS2;
        GPIOA->BSRR = GPIO_BSRR_BS3;
        GPIOA->BSRR = GPIO_BSRR_BS4;

            //turn off led 6-10
        GPIOA->BSRR = GPIO_BSRR_BR5;
        GPIOA->BSRR = GPIO_BSRR_BR6;
        GPIOA->BSRR = GPIO_BSRR_BR7;
        GPIOA->BSRR = GPIO_BSRR_BR8;
        GPIOA->BSRR = GPIO_BSRR_BR9;
    }
    else
    {
            //turn on led 6-10
        GPIOA->BSRR = GPIO_BSRR_BS5;
        GPIOA->BSRR = GPIO_BSRR_BS6;
        GPIOA->BSRR = GPIO_BSRR_BS7;
        GPIOA->BSRR = GPIO_BSRR_BS8;
        GPIOA->BSRR = GPIO_BSRR_BS9;

            //turn off led 1-5
        GPIOA->BSRR = GPIO_BSRR_BR0;
        GPIOA->BSRR = GPIO_BSRR_BR1;
        GPIOA->BSRR = GPIO_BSRR_BR2;
        GPIOA->BSRR = GPIO_BSRR_BR3;
        GPIOA->BSRR = GPIO_BSRR_BR4;
    }

    EXTI->IMR |= EXTI_IMR_IM10;
}

extern "C" void TIM2_IRQHandler()
{
    if(GPIOA->IDR & GPIO_IDR_IDR10)
    {
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR0) ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR1) ? GPIO_BSRR_BR1 : GPIO_BSRR_BS1;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR2) ? GPIO_BSRR_BR2 : GPIO_BSRR_BS2;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR3) ? GPIO_BSRR_BR3 : GPIO_BSRR_BS3;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR4) ? GPIO_BSRR_BR4 : GPIO_BSRR_BS4;
    }
    else
    {
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR5) ? GPIO_BSRR_BR5 : GPIO_BSRR_BS5;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR6) ? GPIO_BSRR_BR6 : GPIO_BSRR_BS6;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR7) ? GPIO_BSRR_BR7 : GPIO_BSRR_BS7;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR8) ? GPIO_BSRR_BR8 : GPIO_BSRR_BS8;
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR9) ? GPIO_BSRR_BR9 : GPIO_BSRR_BS9;
    }

    TIM2->SR &= ~TIM_SR_UIF;
}

int main()
{
        //SETTING LEDs
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL |= GPIO_CRL_MODE0_0;  
    GPIOA->CRL |= GPIO_CRL_MODE1_0;
    GPIOA->CRL |= GPIO_CRL_MODE2_0;
    GPIOA->CRL |= GPIO_CRL_MODE3_0;
    GPIOA->CRL |= GPIO_CRL_MODE4_0;
    GPIOA->CRL |= GPIO_CRL_MODE5_0;
    GPIOA->CRL |= GPIO_CRL_MODE6_0;
    GPIOA->CRL |= GPIO_CRL_MODE7_0;
    GPIOA->CRH |= GPIO_CRH_MODE8_0;
    GPIOA->CRH |= GPIO_CRH_MODE9_0;


        //SETTING INTERRUPT FOR 1-ST BUTTON
    //GPIOA->CRL &= ~(GPIO_CRL_CNF10 | GPIO_CRL_MODE10);
    //GPIOA->CRL &= ~(GPIO_CRL_CNF11 | GPIO_CRL_MODE11);

    AFIO->EXTICR[1] |= AFIO_EXTICR3_EXTI10_PA;
    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->FTSR |= EXTI_FTSR_TR10;

    NVIC_EnableIRQ(EXTI15_10_IRQn);

    EXTI->PR |= EXTI_PR_PR10;
    EXTI->IMR |= EXTI_IMR_IM10;

    EXTI15_10_IRQHandler();


        //SETTING TIMER
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->PSC = 8000 - 1;               // HCLK=8MHz, 8 MHz / (prescaler + 1) = 1000 Hz
    TIM2->ARR = (1000 / 0.8) - 1;       // 1000 Hz / (period + 1) = 2.3 Hz = 2.3 in 1s  (T1 = 2.3)

    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 = TIM_CR1_CEN;


    while (true)
    {
    }
    
    return 0;
}
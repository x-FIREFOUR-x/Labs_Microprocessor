#include "stm32f103x6.h"

    // N = 23 = 23 - 15 = 8;  T1 = N / 10 = 0.8

extern "C" void EXTI15_10_IRQHandler()
{
    if(GPIOA->IDR & GPIO_IDR_IDR10)
    {
            //turn on led 1-5
        GPIOA->BSRR = GPIO_BSRR_BS0;

            //turn off led 6-10
        GPIOA->BSRR = GPIO_BSRR_BR1;
    }
    else
    {
            //turn on led 6-10
        GPIOA->BSRR = GPIO_BSRR_BS1;

            //turn off led 1-5
        GPIOA->BSRR = GPIO_BSRR_BR0;
    }

    EXTI->IMR |= EXTI_IMR_IM10;
}

extern "C" void TIM2_IRQHandler()
{
    if(GPIOA->IDR & GPIO_IDR_IDR10)
    {
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR0) ? GPIO_BSRR_BR0 : GPIO_BSRR_BS0;
    }
    else
    {
        GPIOA->BSRR = (GPIOA->ODR & GPIO_ODR_ODR1) ? GPIO_BSRR_BR1 : GPIO_BSRR_BS1;
    }

    TIM2->SR &= ~TIM_SR_UIF;
}

int main()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN;


        //SETTING LEDs
    GPIOA->CRL |= GPIO_CRL_MODE0_0;  
    GPIOA->CRL |= GPIO_CRL_MODE1_0;

        //SETTING INTERRUPT FOR 1-ST BUTTON
    //GPIOA->CRL &= ~(GPIO_CRL_CNF9 | GPIO_CRL_MODE9);
    //GPIOA->CRL &= ~(GPIO_CRL_CNF10 | GPIO_CRL_MODE10);

    AFIO->EXTICR[1] |= AFIO_EXTICR3_EXTI10_PA;
    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->FTSR |= EXTI_FTSR_TR10;

    NVIC_EnableIRQ(EXTI15_10_IRQn);

    EXTI->PR |= EXTI_PR_PR10;
    EXTI->IMR |= EXTI_IMR_IM10;

    EXTI15_10_IRQHandler();


        //SETTING TIMER
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->PSC = 8000 - 1;               // HCLK=8MHz, 8 MHz / (prescaler + 1) = 1000 Hz
    TIM2->ARR = (1000 / 0.8) - 1;       // 1000 Hz / (period + 1) = 2.3 Hz = 2.3 in 1s  (T1 = 2.3)

    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 = TIM_CR1_CEN;


        //SETTING ШІМ to PA8(base), PA7
    GPIOA->CRH &= ~(GPIO_CRH_CNF8, GPIO_CRH_MODE8);
    GPIOA->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_CNF8_1;

    GPIOA->CRL &= ~(GPIO_CRL_CNF7, GPIO_CRL_MODE7);
    GPIOA->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1;

    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_0;

    TIM1->PSC = 8000 - 1;
    TIM1->ARR = 1000 - 1;
    TIM1->CCR1 = 250 - 1;

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // on ШІМ
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NE; //| TIM_CCER_CC1NP;
    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_CEN;


    while (true)
    {
    }
    
    return 0;
}
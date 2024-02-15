#include "stm32f103x6.h"

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

int main()
{
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
    
    //GPIOA->CRL &= ~(GPIO_CRL_CNF10 | GPIO_CRL_MODE10);
    //GPIOA->CRL &= ~(GPIO_CRL_CNF11 | GPIO_CRL_MODE11);

    AFIO->EXTICR[1] |= AFIO_EXTICR3_EXTI10_PA;
    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->FTSR |= EXTI_FTSR_TR10;
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    EXTI->PR |= EXTI_PR_PR10;
    EXTI->IMR |= EXTI_IMR_IM10;

    EXTI15_10_IRQHandler();

    while (true)
    {
    }
    
    return 0;
}
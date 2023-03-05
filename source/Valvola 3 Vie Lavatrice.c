/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_kbi.h"
#include "switch.h"




/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);
void SysTick_Handler_relocated(void);
void SysTick_Handler(void);
void KBI1_IRQHandler(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool Pulsante_press = false;
volatile uint8_t systick_leds;
volatile uint32_t systick_ticks;
volatile uint32_t systick_ticks1;
volatile uint32_t systick_ticks_counter_ms;
volatile uint32_t u_sec;
volatile uint32_t count_tick;
volatile uint32_t KBI1_ticks;
volatile uint32_t Button1_rising_edges;
volatile uint32_t Button2_rising_edges;
volatile bool Button1_pressed = false;
volatile bool Button2_pressed = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    systick_ticks++;
}
/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/

void SysTick_Handler_relocated(void)
{
    systick_ticks++;
    if (systick_ticks_counter_ms)
    {
        systick_ticks_counter_ms--;
    }

    if (++systick_ticks1 == 200)
    {
        if (systick_leds == 0)  
        {
            GPIO_PortToggle(BOARD_INITLEDSPINS_PTH1_RED_GPIO_PORT, 1u << BOARD_INITLEDSPINS_PTH1_RED_PIN);
        }
        else if (systick_leds == 1)  
        {
            GPIO_PortToggle(BOARD_INITLEDSPINS_PTE7_BLUE_GPIO_PORT, 1u << BOARD_INITLEDSPINS_PTE7_BLUE_PIN);
        }
        else if (systick_leds == 2) 
        {
            GPIO_PortToggle(BOARD_INITLEDSPINS_PTH2_GREEN_GPIO_PORT, 1u << BOARD_INITLEDSPINS_PTH2_GREEN_PIN);
        }
        else
        {
        }
    }
    else if (systick_ticks1 == 400)
    {
        systick_ticks1 = 0;
        GPIO_PortSet(BOARD_INITLEDSPINS_PTH1_RED_GPIO_PORT, BOARD_INITLEDSPINS_PTH1_RED_PIN_MASK);
        GPIO_PortSet(BOARD_INITLEDSPINS_PTE7_BLUE_GPIO_PORT, BOARD_INITLEDSPINS_PTE7_BLUE_PIN_MASK);
        GPIO_PortSet(BOARD_INITPINS_PTH2_GREEN_GPIO_PORT, BOARD_INITLEDSPINS_PTH2_GREEN_PIN_MASK);
    
        if (++systick_leds >= 3)  
        {
            systick_leds = 0;
        }
    }

    SWITCH_handler();
    if (SWITCH_RISING_EDGE == get_Edge_Button1())
    {
        Button1_pressed = true;
    }

    if (SWITCH_RISING_EDGE == get_Edge_Button1())
    {
        Button2_pressed = true;
    }

}

void KBI1_IRQHandler(void)
{
    KBI1_ticks++;
    if (KBI_IsInterruptRequestDetected(KBI1))
    {
        /* Disable interrupts. */
        //KBI_DisableInterrupts(EXAMPLE_KBI);
        /* Clear status. */
        KBI_ClearInterruptFlag(KBI1);
        Pulsante_press = true;
    }
    SDK_ISR_EXIT_BARRIER;
}


void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    kbi_config_t kbiConfig;

    /* Board pin, clock, debug console init */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    void BOARD_InitLEDsPins(void);

    //BOARD_InitDebugConsole();
    PORT_SetFilterDIV3WidthThreshold(PORT, 5);

    kbiConfig.mode        = kKBI_EdgesDetect;
    kbiConfig.pinsEnabled = 1 << 2;
    kbiConfig.pinsEdge    = 0 << 2; /* falling edge.*/
    KBI_Init(KBI1, &kbiConfig);
    KBI_ClearInterruptFlag(KBI1);
    NVIC_SetPriority(KBI1_IRQn, 0x03U);
    
    /* setup systick timer for 1000Hz interrupts */
    if(!InstallIRQHandler(SysTick_IRQn, (uint32_t)SysTick_Handler))
    {
        /* capture error */
        while(1);
    }
    /* setup systick timer for 1000Hz interrupts */
    if(!InstallIRQHandler(SysTick_IRQn, (uint32_t)SysTick_Handler_relocated))
    {
        /* capture error */
        while(1);
    }

    /* setup systick timer for 1000Hz interrupts */
    if(SysTick_Config(SystemCoreClock / 1000U))
    {
        /* capture error */
        while(1);
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x02U);

    SWITCH_init();
    systick_ticks_counter_ms = 1000;
    GPIO_PortSet(BOARD_INITPINS_Led_1_Ora_GPIO_PORT, BOARD_INITPINS_Led_1_Ora_PIN_MASK);
    GPIO_PortSet(BOARD_INITPINS_Led_2_Ora_GPIO_PORT, BOARD_INITPINS_Led_3_Ora_PIN_MASK);
    GPIO_PortSet(BOARD_INITPINS_Led_3_Ora_GPIO_PORT, BOARD_INITPINS_Led_3_Ora_PIN_MASK);

    GPIO_PortSet(BOARD_INITPINS_Led_Fredda_GPIO_PORT, BOARD_INITPINS_Led_Fredda_PIN_MASK);
    GPIO_PortSet(BOARD_INITPINS_Led_Calda_GPIO_PORT, BOARD_INITPINS_Led_Calda_PIN_MASK);
    while (systick_ticks_counter_ms);

/** GPIO_PortClear(BOARD_INITPINS_Led_1_Ora_GPIO_PORT, BOARD_INITPINS_Led_1_Ora_PIN_MASK);*/
    GPIO_PortClear(BOARD_INITPINS_Led_2_Ora_GPIO_PORT, BOARD_INITPINS_Led_3_Ora_PIN_MASK);
    GPIO_PortClear(BOARD_INITPINS_Led_3_Ora_GPIO_PORT, BOARD_INITPINS_Led_3_Ora_PIN_MASK);

/** GPIO_PortClear(BOARD_INITPINS_Led_Fredda_GPIO_PORT, BOARD_INITPINS_Led_Fredda_PIN_MASK);*/
    GPIO_PortClear(BOARD_INITPINS_Led_Calda_GPIO_PORT,  BOARD_INITPINS_Led_Calda_PIN_MASK);


    while (1)
    {
        delay();
        count_tick =  USEC_TO_COUNT(systick_ticks*1000, SystemCoreClock);
        if (Button1_pressed)
        {
            Button1_pressed = false;
            Button1_rising_edges++;
        }
        if (Button2_pressed)
        {
            Button2_pressed = false;
            Button2_rising_edges++;
        }
    }



}


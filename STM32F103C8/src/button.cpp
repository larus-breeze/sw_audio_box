/**
 * @file    button.cpp
 * @brief   Pushbutton example with EXTI interrupt
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "system_configuration.h"

bool blink_slowly;

#if RUN_BUTTON

#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_bus.h"

Semaphore button_activated("BUTTON");

#if BUTTON_ON_PA0

void button_test( void *)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  LL_GPIO_SetPinMode( GPIOA, 0, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull( GPIOA, 0, LL_GPIO_PULL_UP);

  LL_EXTI_EnableFallingTrig_0_31( LL_EXTI_LINE_0);
  LL_EXTI_EnableIT_0_31( LL_EXTI_LINE_0);

  NVIC_SetPriority( EXTI0_IRQn, 15);
  NVIC_EnableIRQ(   EXTI0_IRQn);

  while( true)
    {
      button_activated.wait();

      // any action to be performed shall go into this area

      delay(2);
      LL_EXTI_EnableIT_0_31( LL_EXTI_LINE_0);
      blink_slowly = ! blink_slowly;
    }
}

extern "C" void EXTI0_IRQHandler( void)
{
  if(LL_EXTI_IsActiveFlag_0_31( LL_EXTI_LINE_0) != RESET)
  {
    LL_EXTI_ClearFlag_0_31( LL_EXTI_LINE_0);
    LL_EXTI_DisableIT_0_31( LL_EXTI_LINE_0);

    button_activated.signal_from_ISR();
  }
}

#endif

#if BUTTON_ON_PC13

void button_test( void *)
{
  __HAL_RCC_GPIOC_CLK_ENABLE();

  LL_GPIO_SetPinMode( GPIOC, 13, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull( GPIOC, 13, LL_GPIO_PULL_UP);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_GPIO_AF_SetEXTISource(LL_GPIO_AF_EXTI_PORTC, LL_GPIO_AF_EXTI_LINE13);

  LL_EXTI_EnableFallingTrig_0_31( LL_EXTI_LINE_13);
  LL_EXTI_EnableIT_0_31( LL_EXTI_LINE_13);

  NVIC_SetPriority( EXTI15_10_IRQn, 15);
  NVIC_EnableIRQ(   EXTI15_10_IRQn);

  while( true)
    {
      button_activated.wait();

      // any action to be performed shall go into this area

      delay(2);
      LL_EXTI_EnableIT_0_31( LL_EXTI_LINE_13);
      blink_slowly = ! blink_slowly;
    }
}

extern "C" void EXTI15_10_IRQHandler( void)
{
  if(LL_EXTI_IsActiveFlag_0_31( LL_EXTI_LINE_13) != RESET)
  {
    LL_EXTI_ClearFlag_0_31( LL_EXTI_LINE_13);
    LL_EXTI_DisableIT_0_31( LL_EXTI_LINE_13);

    button_activated.signal_from_ISR();
  }
}

#endif

Task button( button_test,"BUTTON");

#endif

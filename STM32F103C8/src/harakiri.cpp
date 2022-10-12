/**
 * @file    harakiri.cpp
 * @brief   Stack overflow trap demonstration
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
#include "system_configuration.h"

#if SUICIDE_STACKOVERFLOW

/** @brief this task calls itself in a infinite recursion */
static void doit( void)
{
  delay(1);
  doit();
}

/** @brief task code for tack overflow example */
void commit_suicide( void *)
{
  delay( 5000);
  doit();
}

Task suicide( commit_suicide); //!< this task will kill the system

/** @brief this function is implemented in the core system, but it is overloadable */
extern "C" void vApplicationStackOverflowHook (void)
{
  trace_printf( "Mich hat's erwischt !");
  asm("bkpt 0");
}

#endif

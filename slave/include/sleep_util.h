// AVR Multi Motor Control
/*! \file slave/include/sleep_util.h
 * Sleep utility
 *
 * \author Paolo Lucchesi
 */
// NOTE: This interface permanently alters the global interrupt flag
#ifndef __SLEEP_UTIL_INTERFACE_H
#define __SLEEP_UTIL_INTERFACE_H
#include <avr/interrupt.h>
#include <avr/sleep.h>

/*!
 * Enter sleep mode
 *
 * @param mode A sleep mode defined in <avr/sleep.h>
 */
#define sleep(mode) do {\
  uint8_t __sreg_bak = SREG;\
  set_sleep_mode(mode);\
  sleep_enable();\
  sei();\
  sleep_cpu();\
  sleep_disable();\
  SREG = __sreg_bak;\
} while (0);

/*!
 * Enter sleep mode if 'expr' evaluates to true
 *
 * @param mode A sleep mode defined in <avr/sleep.h>
 * @param expr The condition to evaluate
 */
#define sleep_on(mode,expr) do {\
  uint8_t __sreg_bak = SREG;\
  set_sleep_mode(mode);\
  cli();\
  if (expr) {\
    sleep_enable();\
    sei();\
    sleep_cpu();\
    sleep_disable();\
  }\
  SREG = __sreg_bak;\
} while (0)

/*!
 * Continously enter sleep mode while 'expr' evaluates to true
 *
 * @param mode A sleep mode defined in <avr/sleep.h>
 * @param expr The condition to evaluate
 */
#define sleep_while(mode,expr) do {\
  uint8_t __sreg_bak = SREG;\
  set_sleep_mode(mode);\
  while (1) {\
    cli();\
    if (expr) {\
      sleep_enable();\
      sei();\
      sleep_cpu();\
      sleep_disable();\
    }\
    else break;\
  }\
  SREG = __sreg_bak;\
} while (0)

#endif  // __SLEEP_UTIL_INTERFACE_H

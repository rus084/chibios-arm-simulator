/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @file    ARM/chcore.c
 * @brief   ARM7/9 architecture port code.
 *
 * @addtogroup ARM_CORE
 * @{
 */
#include <stdlib.h>
#include "ch.h"
#include "hal.h"
/**
 * Halts the system.
 */
#if !defined(__DOXYGEN__)
__attribute__((weak))
#endif
void port_halt(void) {

  exit(2);
}

/**
 * @brief   Start a thread by invoking its work function.
 * @details If the work function returns @p chThdExit() is automatically
 *          invoked.
 */

/*
__attribute__((cdecl, noreturn))
void _port_thread_start(msg_t (*pf)(void *), void *p) {

  chSysUnlock();
  chThdExit(pf(p));
  while(1);
}
*/
__attribute__((used))
static void __dummy2(msg_t (*pf)(void *), void *p) {
	asm volatile(
	".balign 16			\n\t"
	".code 32			\n\t"
	".globl _port_thread_start	\n\t"
	"_port_thread_start:		\n\t"
#if CH_DBG_SYSTEM_STATE_CHECK
        "mov     r0, #0			\n\t"
        "ldr     r1, =dbg_lock_cnt	\n\t"
        "str     r0, [r1]		\n\t"
#endif
        "msr     CPSR_c, 0x1F	\n\t"
#ifndef THUMB_NO_INTERWORKING
        "mov     r0, r5			\n\t"
        "mov     lr, pc			\n\t"
        "bx      r4			\n\t"
        "bl      chThdExit		\n\t"
#else /* !THUMB_NO_INTERWORKING */
        "add     r0, pc, #1		\n\t"
        "bx      r0			\n\t"
".code 16				\n\t"
        "mov     r0, r5			\n\t"
        "bl      jmpr4			\n\t"
        "bl      chThdExit		\n\t"
"jmpr4:	\n\t"
        "bx      r4	"
#endif /* !THUMB_NO_INTERWORKING */
);
}


__attribute__((used))
static void __dummy(Thread *ntp, Thread *otp) {
  (void)ntp; (void)otp;

  asm volatile (
#if defined(WIN32)
                ".globl @port_switch_arm@8                          \n\t"
                "@port_switch_arm@8:"
#elif defined(__APPLE__)
                ".globl _port_switch_arm                            \n\t"
                "_port_switch_arm:"
#else
                ".globl _port_switch_arm                             \n\t"
                "_port_switch_arm:"
#endif

#if 0
        "stmfd   sp!, {r4, r5, r6, r8, r9, r10, r11, lr}	\n\t"
        "str     sp, [r1, #12]					\n\t"
        "ldr     sp, [r0, #12]					\n\t"
        "ldmfd   sp!, {r4, r5, r6, r8, r9, r10, r11, pc}"
#else // !CH_CURRP_REGISTER_CACHE 
        "stmfd   sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}	\n\t"
        "str     sp, [r1, #12]					\n\t"
        "ldr     sp, [r0, #12]					\n\t"
        "ldmfd   sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}"
#endif // !CH_CURRP_REGISTER_CACHE
	);
}


/** @} */

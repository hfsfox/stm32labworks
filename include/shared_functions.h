/**
 * Copyright (C) 2025 Kravchuk Roman <darkkitsunezx128k2309@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 * Shared functions and defines for MCU's
 */

#ifndef __SHARED_FUNCTIONS_H__
#define __SHARED_FUNCTIONS_H__
#endif

#include <stdint.h>

#ifdef SYSCLOCK
#undef SYSCLOCK
#endif

#define SYSCLOCK_BASE 8000000U

#if defined (__TARGET_STM32F103C8T6__)
#define SYSCLOCK 72000000U
#elif defined (__TARGET_STM32F401CEU6__)
#define SYSCLOCK 84000000U
#elif defined (__TARGET_STM32F411CCU6__)
#define SYSCLOCK 100000000U
#else
#define SYSCLOCK SYSCLOCK_BASE
#endif

void _set_dsb(void);
void _delay_ticks(volatile uint32_t ticks);
void _delay_msecs(volatile uint32_t ms);
void _delay_usecs(volatile uint32_t ms);
void _work_fq_setup(void);
void _sys_init_external_clk(void);
void _sys_init_internal_clk(void);

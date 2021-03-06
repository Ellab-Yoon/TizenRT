/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * arch/arm/src/armv7-m/arch_enable_dcache.c
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Some logic in this header file derives from the ARM CMSIS core_cm7.h
 * header file which has a compatible 3-clause BSD license:
 *
 *   Copyright (c) 2009 - 2014 ARM LIMITED.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name ARM, TinyARA nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>

#include "cache.h"

#ifdef CONFIG_ARMV7M_DCACHE

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arch_enable_dcache
 *
 * Description:
 *   Enable the D-Cache
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void arch_enable_dcache(void)
{
	uint32_t ccsidr;
	uint32_t ccr;
	uint32_t sshift;
	uint32_t wshift;
	uint32_t sw;
	uint32_t sets;
	uint32_t ways;

	/* Get the characteristics of the D-Cache */

	ccsidr = getreg32(NVIC_CCSIDR);
	sets = CCSIDR_SETS(ccsidr);	/* (Number of sets) - 1 */
	sshift = CCSIDR_LSSHIFT(ccsidr) + 4;	/* log2(cache-line-size-in-bytes) */
	ways = CCSIDR_WAYS(ccsidr);	/* (Number of ways) - 1 */

	/* Calculate the bit offset for the way field in the DCISW register by
	 * counting the number of leading zeroes.  For example:
	 *
	 *   Number of  Value of ways  Field
	 *   Ways       'ways'         Offset
	 *     2         1             31
	 *     4         3             30
	 *     8         7             29
	 *   ...
	 */

	wshift = arm_clz(ways) & 0x1f;

	/* Invalidate the entire D-Cache */

	ARM_DSB();
	do {
		int32_t tmpways = ways;

		do {
			sw = ((tmpways << wshift) | (sets << sshift));
			putreg32(sw, NVIC_DCISW);
		} while (tmpways--);
	} while (sets--);

	ARM_DSB();

	/* Enable the D-Cache */

	ccr = getreg32(NVIC_CFGCON);
	ccr |= NVIC_CFGCON_DC;
	putreg32(ccr, NVIC_CFGCON);

	ARM_DSB();
	ARM_ISB();
}

#endif							/* CONFIG_ARMV7M_DCACHE */

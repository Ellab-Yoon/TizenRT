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
 * arch/arm/src/armv7-m/up_memfault.c
 *
 *   Copyright (C) 2011, 2013, 2018 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 * 3. Neither the name NuttX nor the names of its contributors may be
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

#include <assert.h>
#include <debug.h>

#include <arch/irq.h>

#include "up_arch.h"
#include "nvic.h"
#include "up_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_DEBUG_MEMFAULT
#define mferr(format, ...)  _alert(format, ##__VA_ARGS__)
#define mfinfo(format, ...) _alert(format, ##__VA_ARGS__)
#else
#define mferr(x...)
#define mfinfo(x...)
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_memfault
 *
 * Description:
 *   This is Memory Management Fault exception handler.  Normally we get here
 *   when the Cortex M3 MPU is enabled and an MPU fault is detected.  However,
 *   I understand that there are other error conditions that can also generate
 *   memory management faults.
 *
 ****************************************************************************/

int up_memfault(int irq, FAR void *context, FAR void *arg)
{
	/* Dump some memory management fault info */

	(void)irqsave();
	lldbg("PANIC!!! Memory Management Fault:\n");
	lldbg("  IRQ: %d context: %p\n", irq, context);
	lldbg("  CFAULTS: %08x MMFAR: %08x\n",
		  getreg32(NVIC_CFAULTS), getreg32(NVIC_MEMMANAGE_ADDR));
	lldbg("  BASEPRI: %08x PRIMASK: %08x IPSR: %08x CONTROL: %08x\n",
		  getbasepri(), getprimask(), getipsr(), getcontrol());

#ifdef CONFIG_ARMV7M_USEBASEPRI
#ifdef REG_EXC_RETURN
	lldbg("  xPSR: %08x BASEPRI: %08x EXC_RETURN: %08x (saved)\n",
		  current_regs[REG_XPSR], current_regs[REG_BASEPRI], current_regs[REG_EXC_RETURN]);
#else
	lldbg("  xPSR: %08x BASEPRI: %08x (saved)\n",
		  current_regs[REG_XPSR], current_regs[REG_BASEPRI]);
#endif
#else
#ifdef REG_EXC_RETURN
	lldbg("  xPSR: %08x PRIMASK: %08x EXC_RETURN: %08x (saved)\n",
	      current_regs[REG_XPSR], current_regs[REG_PRIMASK], current_regs[REG_EXC_RETURN]);
#else
	lldbg("  xPSR: %08x PRIMASK: %08x (saved)\n", current_regs[REG_XPSR], current_regs[REG_PRIMASK]);
#endif
#endif

	PANIC();
	return OK;					/* Won't get here */
}

/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* IRQ entry wrappers: save what a C call may clobber, call the C body,
 * restore, csync, rti. clang does not emit the csync needed before rti,
 * so ISRs are not written in C directly. */
	.macro FM1_ISR name, body, segment
	.section \segment,"ax",@progbits
	.globl \name
\name:
	[--sp] = {psr, rets, reti}
	[--sp] = {r3-r0}
	/* The far XIP C handler is ~4 MiB from RAM and cannot use a
	 * relative 23-bit call; form its 32-bit address and call indirectly. */
	r0 = #\body
	call r0
	{r3-r0} = [sp++]
	{psr, rets, reti} = [sp++]
	csync
	rti
	.endm

	/* Hardware diagnostic: execute the short 10 kHz IRQ entry/exit wrapper
 * from on-chip RAM instead of XIP flash. Previous crash PC 030004A8 was
 * a single high-bit flip of the XIP timer wrapper second instruction,
 * while the IRQ63 vector itself was correct. Keeping this short wrapper
 * in RAM isolates flash-fetch/bus-contention as a possible cause without
 * modifying the keyboard scan routine, ISR priority or 2-core audio DSP. */
	FM1_ISR isr_timer5, fm1_timer5_irq, .ram_text.isr_timer5
	FM1_ISR isr_alnk0, fm1_alnk0_irq, .text.isr_alnk0

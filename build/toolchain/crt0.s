@---------------------------------------------------------------------------------
@ crt0.s - GBA startup code
@ Minimal startup code for Game Boy Advance
@---------------------------------------------------------------------------------
	.section .crt0,"ax"
	.global _start
	.align 4
	.arm

_start:
	b	rom_header_end

	.fill	156,1,0				@ Nintendo Logo (156 bytes)

	.fill	12,1,0				@ Game Title (12 bytes)
	.fill	4,1,0				@ Game Code (4 bytes)
	.ascii	"01"				@ Maker Code (2 bytes)
	.byte	0x96				@ Fixed value
	.byte	0x00				@ Main unit code
	.byte	0x00				@ Device type
	.fill	7,1,0				@ Reserved (7 bytes)
	.byte	0x00				@ Software version
	.byte	0x00				@ Complement check (filled by gbafix)
	.fill	2,1,0				@ Reserved (2 bytes)

rom_header_end:
	b	start_vector

	.fill	4,1,0				@ Reserved

start_vector:
	@ Switch to IRQ Mode
	mov	r0, #0x12
	msr	cpsr, r0
	ldr	sp, =__sp_irq

	@ Switch to System Mode
	mov	r0, #0x1f
	msr	cpsr, r0
	ldr	sp, =__sp_usr

	@ Clear BSS section
	ldr	r0, =__bss_start__
	ldr	r1, =__bss_end__
	mov	r2, #0
.clear_bss:
	cmp	r0, r1
	strlt	r2, [r0], #4
	blt	.clear_bss

	@ Copy data section from ROM to RAM
	ldr	r0, =__data_start
	ldr	r1, =__data_end
	ldr	r2, =__data_lma
.copy_data:
	cmp	r0, r1
	ldrlt	r3, [r2], #4
	strlt	r3, [r0], #4
	blt	.copy_data

	@ Jump to main
	ldr	r0, =main
	bx	r0

	@ Infinite loop if main returns
.loop:
	b	.loop

.pool
.end

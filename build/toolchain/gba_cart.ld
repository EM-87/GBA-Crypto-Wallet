/*
 * gba_cart.ld - Linker script for Game Boy Advance cartridge
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY {
	rom	: ORIGIN = 0x08000000, LENGTH = 32M
	iwram	: ORIGIN = 0x03000000, LENGTH = 32K
	ewram	: ORIGIN = 0x02000000, LENGTH = 256K
}

SECTIONS {
	.crt0 : {
		KEEP (*(.crt0))
		. = ALIGN(4);
	} >rom

	.text : {
		*(.text)
		*(.text.*)
		*(.gnu.linkonce.t.*)
		*(.glue_7)
		*(.glue_7t)
		. = ALIGN(4);
	} >rom

	.rodata : {
		*(.rodata)
		*(.rodata.*)
		*(.gnu.linkonce.r.*)
		. = ALIGN(4);
	} >rom

	__data_lma = .;

	.data : {
		__data_start = .;
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(4);
		__data_end = .;
	} >ewram AT>rom

	.bss (NOLOAD) : {
		__bss_start__ = .;
		*(.bss)
		*(.bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} >ewram

	/* Stack setup */
	__sp_irq = ORIGIN(iwram) + LENGTH(iwram) - 0x100;
	__sp_usr = __sp_irq - 0x100;

	/* Heap in external RAM */
	__heap_start = ORIGIN(ewram);
	__heap_end = ORIGIN(ewram) + LENGTH(ewram);

	/* Discard debug sections */
	/DISCARD/ : {
		*(.comment)
		*(.debug*)
	}
}

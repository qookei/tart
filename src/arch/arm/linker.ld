EXTERN (vector_table)
ENTRY (reset_handler)

SECTIONS {
	.pre : {
		KEEP (*(.pre))
	} >rom

	.text : {
		*(.vectors*)
		*(.text*)
		. = ALIGN(4);
		*(.rodata*)
		. = ALIGN(4);
	} >rom

	.init_array : {
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	} >rom

	. = ALIGN(4);
	.data : {
		__data_start = .;
		*(.data*)
		__data_end = .;
		. = ALIGN(4);
	} >ram AT >rom
	__data_loadaddr = LOADADDR(.data);

	.bss : {
		__bss_start = .;
		*(.bss*)
		*(COMMON)
		__bss_end = .;
		. = ALIGN(4);
	} >ram

	/DISCARD/ : {
		*(.eh_frame)
		*(.ARM.extab*)
		*(.ARM.exidx*)
	}

	.image_ram_end : {
		. = ALIGN(4);
		__image_ram_end = .;
	} >ram
}

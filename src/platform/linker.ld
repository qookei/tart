EXTERN (vector_table)
ENTRY(reset_handler)

SECTIONS {
	.text : {
		*(.vectors*)
		*(.text*)
		. = ALIGN(4);
		*(.rodata*)
		. = ALIGN(4);
	} >rom

	.init_array : {
		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;
	} >rom

	. = ALIGN(4);
	_etext = .;

	.data : {
		_data_start = .;
		*(.data*)
		. = ALIGN(4);
		_data_end = .;
	} >ram AT >rom
	_data_loadaddr = LOADADDR(.data);

	.bss : {
		_bss_start = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_bss_end = .;
	} >ram

	/DISCARD/ : {
		*(.eh_frame)
		*(.ARM.extab*)
		*(.ARM.exidx*)
	}

	. = ALIGN(4);
	image_end = .;
}

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));

MEMORY {
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

PROVIDE(core0_stack = ORIGIN(ram) + LENGTH(ram));

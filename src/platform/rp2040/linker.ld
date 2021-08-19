MEMORY {
	rom (rx) : ORIGIN = 0x10000000, LENGTH = 2048k
	ram (rwx) : ORIGIN =  0x20000000, LENGTH = 256k

	scratch0 (rwx) : ORIGIN = 0x20040000, LENGTH = 4k
	scratch1 (rwx) : ORIGIN = 0x20041000, LENGTH = 4k
}

PROVIDE(core0_stack = ORIGIN(scratch0) + LENGTH(scratch0));
PROVIDE(core1_stack = ORIGIN(scratch1) + LENGTH(scratch1));

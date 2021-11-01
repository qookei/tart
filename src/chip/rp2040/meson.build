ld_path = meson.current_source_dir() + '/linker.ld'
link_args += ['-T', ld_path]
link_depends += files(ld_path)

c = find_program('c')

stage1_blob_gen = generator(
	find_program('make-stage1-blob.py'),
	arguments : [c.full_path(), objcopy.full_path(), '@INPUT@', '@OUTPUT@'],
	output : 'blob-@BASENAME@.h')

sources += files(
	'stage1.cpp',
	'platform.cpp',
	'resets.cpp',
	'clocks.cpp',
	'uart.cpp',
	'irq.cpp',
	'memory.cpp',
	'atomic.cpp'
)

sources += stage1_blob_gen.process('stage1/stage1-w25q080.S')

cpp_args += '-DTART_CHIP_RP2040'

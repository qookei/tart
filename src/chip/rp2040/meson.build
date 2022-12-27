ld_path = meson.current_source_dir() + '/linker.ld'
link_args += ['-T', ld_path]
link_depends += files(ld_path)

c = find_program('c')

sources += files(
	'irq.cpp',
	'atomic.cpp'
)

cpp_args += '-DTART_CHIP_RP2040'

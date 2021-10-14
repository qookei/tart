if chip == 'stm32f103x8'
	arch = 'arm'
	ld_path = meson.current_source_dir() + '/stm32f103x8/linker.ld'
	link_args += ['-T', ld_path]
	link_depends += files(ld_path)

	sources += files(
		'stm32f103x8/platform.cpp',
		'stm32f103x8/rcc.cpp',
		'stm32f103x8/flash.cpp',
		'stm32f103x8/usart.cpp',
		'stm32f103x8/spi.cpp',
		'stm32f103x8/gpio.cpp'
	)
elif chip == 'rp2040'
	arch = 'arm'
	subdir('rp2040')
endif

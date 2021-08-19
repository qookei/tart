platform_sources = files('init.cpp')
main_platform_ld = meson.current_source_dir() + '/linker.ld'

if this_platform == 'stm32f103x8'
	platform_sources += files(
		'stm32f103x8/platform.cpp',
		'stm32f103x8/rcc.cpp',
		'stm32f103x8/flash.cpp',
		'stm32f103x8/usart.cpp',
		'stm32f103x8/spi.cpp',
		'stm32f103x8/gpio.cpp'
	)
elif this_platform == 'rp2040'
	subdir('rp2040')
endif

platform_sources = files('init.cpp')
this_platform_ld = meson.current_source_dir() + '/@0@/linker.ld'.format(this_platform)
platform_ld = meson.current_source_dir() + '/linker.ld'

if this_platform == 'stm32f103x8'
	platform_sources += files(
		'stm32f103x8/platform.cpp',
		'stm32f103x8/rcc.cpp',
		'stm32f103x8/flash.cpp',
		'stm32f103x8/usart.cpp',
		'stm32f103x8/gpio.cpp'
	)
endif

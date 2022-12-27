if arch == 'arm'
	ld_path = meson.current_source_dir() + '/arm/linker.ld'
	link_args += ['-T', ld_path]
	link_depends += files(ld_path)

	sources += files('arm/handlers.cpp') # 'arm/init.cpp', 'arm/systick.cpp'
endif

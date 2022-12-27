if arch == 'arm'
	ld_path = meson.current_source_dir() + '/arm/linker.ld'
	link_args += ['-T', ld_path]
	link_depends += files(ld_path)

	sources += files('arm/memory.cpp', 'arm/handlers.cpp', 'arm/systick.cpp') # 'arm/init.cpp', 'arm/vector.cpp'
endif

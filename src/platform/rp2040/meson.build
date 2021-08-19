this_platform_ld = meson.current_source_dir() + '/linker.ld'

c = find_program('c')

stage1_blob_gen = generator(
	find_program('make-stage1-blob.py'),
	arguments : [c.full_path(), objcopy.full_path(), '@INPUT@', '@OUTPUT@'],
	output : 'blob-@BASENAME@.h')

platform_sources += files(
	'stage1.cpp',
	'platform.cpp',
	'resets.cpp',
	'clocks.cpp',
	'uart.cpp'
)

platform_sources += stage1_blob_gen.process('stage1/stage1-w25q080.S')

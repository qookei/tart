project('tart', 'cpp',
	version: '0.1',
	default_options: [
		'warning_level=2',
		'buildtype=minsize',
#		'b_lto=true',
		'cpp_std=c++2a'])

objcopy = find_program('objcopy')
size = find_program('size')

this_platform = get_option('platform')
inc = include_directories('src')

subdir('src/platform')

cpp_args = ['-fcoroutines-ts', '-fno-exceptions', '-fno-rtti']
link_args = ['-T', this_platform_ld, '-T', platform_ld]

libarch = subproject('libarch', default_options: ['install_headers=false'])
libarch_dep = libarch.get_variable('libarch_dep')

sources = [
	platform_sources,
	files(
		'src/lib/string.cpp'
	)
]

elf = executable('tart.elf', sources,
		include_directories: inc,
		dependencies: [libarch_dep],
		cpp_args: [cpp_args],
		link_args: [link_args],
		link_depends: [files(platform_ld, this_platform_ld)])

bin = custom_target('tart.bin',
		input: elf, output: 'tart.bin',
		build_by_default: true,
		command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'])

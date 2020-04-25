project('tart', 'cpp',
	version: '0.1',
	default_options: [
		'warning_level=2',
		'b_lto=true',
		'buildtype=minsize',
		'cpp_std=c++2a'])

objcopy = find_program('objcopy')
size = find_program('size')

this_platform = get_option('platform')
inc = include_directories('src')

subdir('src/platform')

cpp_args = ['-fcoroutines-ts', '-fno-exceptions', '-fno-rtti']
link_args = ['-T', this_platform_ld, '-T', platform_ld]

frigg = subproject('frigg', default_options: ['frigg_no_install=true'])
cxxshim = subproject('cxxshim')
libarch = subproject('libarch', default_options: ['install_headers=false'])

frigg_dep = frigg.get_variable('frigg_dep')
cxxshim_dep = cxxshim.get_variable('cxxshim_dep')
cxxshim_clang_coro_dep = cxxshim.get_variable('clang_coroutine_dep')
libarch_dep = libarch.get_variable('libarch_dep')

sources = [
	platform_sources,
	files(
		'src/lib/string.cpp',
		'src/lib/logger.cpp',
		'src/mem/mem.cpp'
	)
]

elf = executable('tart.elf', sources,
		include_directories: inc,
		dependencies: [frigg_dep, cxxshim_dep, cxxshim_clang_coro_dep, libarch_dep],
		cpp_args: [cpp_args],
		link_args: [link_args, '-lgcc'],
		link_depends: [files(platform_ld, this_platform_ld)])

bin = custom_target('tart.bin',
		input: elf, output: 'tart.bin',
		build_by_default: true,
		command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'])

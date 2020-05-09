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

cpp_args = ['-fcoroutines', '-fno-exceptions', '-fno-rtti', '-fno-threadsafe-statics',
		'-DCXXSHIM_INTEGRATE_GCC', '-DLIBASYNC_CUSTOM_PLATFORM', '-fno-omit-frame-pointer']
link_args = ['-T', this_platform_ld, '-T', platform_ld]

frigg = subproject('frigg', default_options: ['frigg_no_install=true'])
cxxshim = subproject('cxxshim')
libarch = subproject('libarch', default_options: ['install_headers=false'])

frigg_dep = frigg.get_variable('frigg_dep')
cxxshim_dep = cxxshim.get_variable('cxxshim_dep')
cxxshim_std_coro_dep = cxxshim.get_variable('std_coroutine_dep')
libarch_dep = libarch.get_variable('libarch_dep')
libasync_inc = include_directories('subprojects/libasync/include')

sources = [
	platform_sources,
	files(
		'src/lib/string.cpp',
		'src/lib/logger.cpp',
		'src/mem/mem.cpp',
		'src/drivers/w25x.cpp',
		'src/drivers/enc28j60.cpp',
		'src/net/process.cpp',
		'src/service.cpp',
		'src/cxx-support.cpp'
	)
]

elf = executable('tart.elf', sources,
		include_directories: [inc, libasync_inc],
		dependencies: [frigg_dep, cxxshim_dep, cxxshim_std_coro_dep, libarch_dep],
		cpp_args: [cpp_args],
		link_args: [link_args, '-lgcc'],
		link_depends: [files(platform_ld, this_platform_ld)])

bin = custom_target('tart.bin',
		input: elf, output: 'tart.bin',
		build_by_default: true,
		command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'])

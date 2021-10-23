project('tart', 'cpp',
	version: '0.1',
	default_options: [
		'warning_level=3',
		#		'b_lto=true',
		'buildtype=minsize',
		'cpp_std=c++20'])

objcopy = find_program('objcopy')

arch = 'user'
chip = get_option('platform')

includes = include_directories('include', 'src')
sources = []

cpp_args = ['-fno-exceptions', '-fno-rtti', '-fno-threadsafe-statics',
		'-DLIBASYNC_CUSTOM_PLATFORM', '-fno-omit-frame-pointer']
link_args = ['-lgcc']
link_depends = []

subdir('src/chip')
subdir('src/arch')

if meson.get_compiler('cpp').has_argument('-fcoroutines')
	cpp_args += ['-fcoroutines']
elif meson.get_compiler('cpp').has_argument('-fcoroutines-ts')
	cpp_args += ['-fcoroutines-ts']
else
	error('unsupported compiler')
endif

frigg = subproject('frigg', default_options: ['frigg_no_install=true'])
cxxshim = subproject('cxxshim')
libarch = subproject('libarch', default_options: ['install_headers=false'])

frigg_dep = frigg.get_variable('frigg_dep')
cxxshim_dep = cxxshim.get_variable('cxxshim_dep')

cxxshim_coro_dep_name = ''
if meson.get_compiler('cpp').get_id() == 'clang'
	cxxshim_coro_dep_name = 'clang_coroutine_dep'
else
	cxxshim_coro_dep_name = 'std_coroutine_dep'
endif

cxxshim_coro_dep = cxxshim.get_variable(cxxshim_coro_dep_name)

if meson.get_compiler('cpp').get_id() == 'clang'
	# FIXME: clang with "-target arm-none-eabi" doesn't seem to care
	#        about the gcc toolchain even if we explicitly tell it about
	#        one, the short-term fix is hardcoding the include and library paths
	link_args += ['-L/usr/lib/gcc/arm-none-eabi/11.1.0']


	# enable some functions clang emits calls to that would be found in compiler-rt
	# (these functions are __aeabi_mem{cpy,clr,move}, which are like mem{cpy,set,move})
	cpp_args += ['-DCLANG_MEMES']

	# override libasync check for which coroutine header to use (since both the gcc
	# and the clang one are in the include path due to the -I... arguments passed above)
	cpp_args += ['-DLIBASYNC_FORCE_USE_EXPERIMENTAL']
endif

if get_option('enc28j60-verbose')
	cpp_args += ['-DENC28J60_VERBOSE']
endif

if get_option('enc28j60-tell-me-more')
	cpp_args += ['-DENC28J60_TELL_ME_MORE']
endif

libarch_dep = libarch.get_variable('libarch_dep')
libasync_inc = include_directories('subprojects/libasync/include')

sources += [
	files(
		'src/lib/string.cpp',
		'src/lib/logger.cpp',
		'src/mem/mem.cpp',
		#		'src/drivers/w25x.cpp',
		#		'src/drivers/enc28j60.cpp',
		#'src/net/arp.cpp',
		#		'src/net/ipv4/icmp.cpp',
		#		'src/net/ipv4/udp.cpp',
		#		'src/net/ipv4/tcp.cpp',
		'src/cxx-support.cpp',
		'src/init.cpp',
		'src/log.cpp'
	)
]

lib = static_library('tart', sources,
		include_directories: [includes, libasync_inc],
		dependencies: [frigg_dep, cxxshim_dep, cxxshim_coro_dep, libarch_dep],
		cpp_args: cpp_args,
		link_args: link_args,
		link_depends: link_depends,
		pic: false)

tart_dep = declare_dependency(link_whole: lib,
	dependencies: [frigg_dep, cxxshim_dep, cxxshim_coro_dep, libarch_dep],
	link_args: link_args,
	compile_args: cpp_args,
	link_depends: link_depends,
	include_directories: [includes, libasync_inc])

subdir('examples/hello')

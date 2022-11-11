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

includes = include_directories('include', 'include/compat')
sources = []

cpp_args = ['-fno-exceptions', '-fno-rtti', '-fno-threadsafe-statics',
		'-DLIBASYNC_CUSTOM_PLATFORM', '-fno-omit-frame-pointer',
		'-fno-use-cxa-atexit']
link_args = ['-lgcc']
link_depends = []

subdir('src/chip')
subdir('src/arch')

frigg = subproject('frigg', default_options: ['frigg_no_install=true'])
cxxshim = subproject('cxxshim')
libarch = subproject('libarch', default_options: ['install_headers=false'])

frigg_dep = frigg.get_variable('frigg_dep')
cxxshim_dep = cxxshim.get_variable('cxxshim_dep')

cxxshim_coro_dep = cxxshim.get_variable('std_coroutine_dep')

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
		'src/alloc.cpp',
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


tart_core_dep = declare_dependency(
	dependencies: [frigg_dep, cxxshim_dep, cxxshim_coro_dep, libarch_dep],
	compile_args: cpp_args,
	include_directories: [includes, libasync_inc])

lib = static_library('tart', sources,
		dependencies: tart_core_dep,
		cpp_args: cpp_args,
		link_args: link_args,
		link_depends: link_depends,
		pic: false)

subdir('drivers/')

tart_dep = declare_dependency(link_whole: lib,
	dependencies: [tart_core_dep, rp2_gpio_dep, rp2_resets_dep, pl011_uart_dep, rp2_osc_dep, rp2_pll_dep, rp2_clk_dep],
	link_args: link_args,
	compile_args: cpp_args,
	include_directories: [includes, libasync_inc])

subdir('examples/hello')

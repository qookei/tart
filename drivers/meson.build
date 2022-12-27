# -----------------------------------------------------------------------------
# gpio
# -----------------------------------------------------------------------------

rp2_gpio_lib = static_library('tart-rp2-gpio', 'gpio/rp2-gpio.cpp',
	dependencies : tart_core_dep)
rp2_gpio_dep = declare_dependency(link_with : rp2_gpio_lib)

# -----------------------------------------------------------------------------
# resets
# -----------------------------------------------------------------------------

rp2_resets_lib = static_library('tart-rp2-resets', 'reset/rp2-resets.cpp',
	dependencies : tart_core_dep)
rp2_resets_dep = declare_dependency(link_with : rp2_resets_lib)

# -----------------------------------------------------------------------------
# uart
# -----------------------------------------------------------------------------

pl011_uart_lib = static_library('tart-pl011-uart', 'uart/pl011.cpp',
	dependencies : tart_core_dep)
pl011_uart_dep = declare_dependency(link_with : pl011_uart_lib)

# -----------------------------------------------------------------------------
# clk
# -----------------------------------------------------------------------------

rp2_osc_lib = static_library('tart-rp2-osc', 'clk/rp2-osc.cpp',
	dependencies : tart_core_dep)
rp2_osc_dep = declare_dependency(link_with : rp2_osc_lib)

rp2_pll_lib = static_library('tart-rp2-pll', 'clk/rp2-pll.cpp',
	dependencies : tart_core_dep)
rp2_pll_dep = declare_dependency(link_with : rp2_pll_lib)

rp2_clk_lib = static_library('tart-rp2-clk', 'clk/rp2-clk.cpp',
	dependencies : tart_core_dep)
rp2_clk_dep = declare_dependency(link_with : rp2_clk_lib)

# -----------------------------------------------------------------------------
# boot
# -----------------------------------------------------------------------------

freestanding_startup_lib = static_library('tart-freestanding-startup', 'boot/freestanding-startup.cpp',
	dependencies : tart_core_dep)
freestanding_startup_dep = declare_dependency(link_with : freestanding_startup_lib)

subdir('boot/rp2-stage1')

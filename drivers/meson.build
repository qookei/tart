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

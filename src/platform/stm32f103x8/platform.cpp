#include "../platform.hpp"
#include "rcc.hpp"
#include "usart.hpp"
#include "gpio.hpp"

void platform::setup() {
	rcc::clock_setup_ext_8mhz_72mhz();
	usart::init(1, 115200, usart::parity::none, usart::stop_bits::one);
}

void platform::run() {
	gpio::setup_pin(gpio::bank::b, 4, gpio::mode::output_50mhz, gpio::conf::push_pull);
	for (int j = 0; j < 20; j++) {
		gpio::set_pin(gpio::bank::b, 4, j & 1);
		for (int i = 0; i < 800000; i++)
			asm volatile("nop");
	}
	while(true)
		usart::send(1, "Hello, world!\r\n", 15);
}

#include "spi.hpp"
#include "spi_reg.hpp"

#include "rcc.hpp"
#include <periph/gpio.hpp>

namespace platform::spi {

void init(int nth, int clock_divisor, int mode, bool msb, size s) {
	auto &spi = spi_space[nth - 1];

	// TODO: determine based on nth

	switch(nth) {
		case 1:
			gpio::setup(gpio::pa5, gpio::mode::output_50mhz, gpio::config::push_pull, true); // SCK
			gpio::setup(gpio::pa6, gpio::mode::input, gpio::config::floating); // MISO
			gpio::setup(gpio::pa7, gpio::mode::output_50mhz, gpio::config::push_pull, true); // MOSI
			rcc::set_periph_enable(rcc::periph::spi1, true);
			break;
		case 2:
			gpio::setup(gpio::pb13, gpio::mode::output_50mhz, gpio::config::push_pull, true); // SCK
			gpio::setup(gpio::pb14, gpio::mode::input, gpio::config::floating); // MISO
			gpio::setup(gpio::pb15, gpio::mode::output_50mhz, gpio::config::push_pull, true); // MOSI
			rcc::set_periph_enable(rcc::periph::spi2, true);
			break;
	}

	int div = __builtin_ctz(clock_divisor) - 1;

	spi.store(reg::cr2, spi.load(reg::cr2)
			/ cr2::ss_oe(false));
	spi.store(reg::cr1, spi.load(reg::cr1)
			/ cr1::bidi_mode(false)
			/ cr1::bidi_oe(false)
			/ cr1::crc_enable(false)
			/ cr1::crc_next(false)
			/ cr1::word_length(s)
			/ cr1::rx_only(false)
			/ cr1::ssm(false)
			/ cr1::ssi(false)
			/ cr1::msb_first(msb)
			/ cr1::enable(true)
			/ cr1::clock_divisor(div)
			/ cr1::master(true)
			/ cr1::clock_polarity((mode >> 1) & 1)
			/ cr1::clock_phase(mode & 1));
}

uint16_t transfer(int nth, uint16_t value) {
	auto &spi = spi_space[nth - 1];

	spi.store(reg::data, value);
	while (!(spi.load(reg::status) & status::read_full));

	return spi.load(reg::data);
}

} // namespace platform::spi

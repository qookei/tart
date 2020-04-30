#include <periph/spi.hpp>
#include "spi_reg.hpp"

#include "rcc.hpp"
#include <periph/gpio.hpp>

namespace spi {

static constexpr arch::mem_space spi_space[2] = {
	0x40013000,
	0x40003800
};

void spi::setup(int clock_div, int mode, bool msb) {
	auto &spi = spi_space[nth_];

	using namespace platform;

	switch(nth_) {
		case 0:
			gpio::setup(gpio::pa5, gpio::mode::output_50mhz, gpio::config::push_pull, true); // SCK
			gpio::setup(gpio::pa6, gpio::mode::input, gpio::config::floating); // MISO
			gpio::setup(gpio::pa7, gpio::mode::output_50mhz, gpio::config::push_pull, true); // MOSI
			rcc::set_periph_enable(rcc::periph::spi1, true);
			break;
		case 1:
			gpio::setup(gpio::pb13, gpio::mode::output_50mhz, gpio::config::push_pull, true); // SCK
			gpio::setup(gpio::pb14, gpio::mode::input, gpio::config::floating); // MISO
			gpio::setup(gpio::pb15, gpio::mode::output_50mhz, gpio::config::push_pull, true); // MOSI
			rcc::set_periph_enable(rcc::periph::spi2, true);
			break;
	}

	int div = __builtin_ctz(clock_div) - 1;

	spi.store(reg::cr2, spi.load(reg::cr2)
			/ cr2::ss_oe(false));
	spi.store(reg::cr1, spi.load(reg::cr1)
			/ cr1::bidi_mode(false)
			/ cr1::bidi_oe(false)
			/ cr1::crc_enable(false)
			/ cr1::crc_next(false)
			/ cr1::word_length(false)
			/ cr1::rx_only(false)
			/ cr1::ssm(false)
			/ cr1::ssi(false)
			/ cr1::lsb_first(!msb)
			/ cr1::enable(true)
			/ cr1::clock_divisor(div)
			/ cr1::master(true)
			/ cr1::clock_polarity((mode >> 1) & 1)
			/ cr1::clock_phase(mode & 1));
}

bool spi::is_transmit_empty() {
	auto &spi = spi_space[nth_];
	return spi.load(reg::status) & status::transmit_empty;
}

bool spi::is_receive_not_empty() {
	auto &spi = spi_space[nth_];
	return spi.load(reg::status) & status::receive_not_empty;
}

void spi::send(uint8_t value) {
	auto &spi = spi_space[nth_];
	spi.store(reg::data, value);
}

uint8_t spi::recv() {
	auto &spi = spi_space[nth_];
	return spi.load(reg::data);
}

void spi::select() {
	// while (!atomic test and set)
	//	co_await was_deselected.async_wait();
}

void spi::deselect() {
	// was_deselected.ring();
	// atomic clear;
}

spi *get_spi(int nth) {
	static spi this_platform_spi_[2] = {{0}, {1}};

	if (nth < 1 || nth > 2)
		return nullptr;

	return &this_platform_spi_[nth - 1];
}

} // namespace spi

#include "usart.hpp"
#include "usart_reg.hpp"

#include "rcc.hpp"
#include "gpio.hpp"

namespace platform::usart {

void init(int nth, int baud, parity par, stop_bits stop) {
	auto &usart = usart_space[nth - 1];

	gpio::setup_pin(gpio::bank::a, 9, gpio::mode::output_50mhz, gpio::conf::af_push_pull);

	// TODO: get from rcc code
	constexpr int apb2_clock = 72000000;
	constexpr int apb1_clock = 36000000;

	auto this_clock = apb2_clock;
	if (nth > 1)
		this_clock = apb1_clock;

	bool par_en = false;
	bool par_mod = false;

	if (par != parity::none) {
		par_en = true;
		par_mod = par == parity::odd;
	}

	switch(nth) {
		case 1:
			rcc::set_periph_enable(rcc::periph::usart1, true);
			break;
		case 2:
			rcc::set_periph_enable(rcc::periph::usart2, true);
			break;
		case 3:
			rcc::set_periph_enable(rcc::periph::usart3, true);
			break;
	}

	usart.store(reg::baud, (this_clock + baud / 2) / baud);
	usart.store(reg::cr1, usart.load(reg::cr1)
			| cr1::word_length(false)
			| cr1::parity_enable(par_en)
			| cr1::parity(par_mod)
			| cr1::transmit_enable(true)
			| cr1::receive_enable(true));
	usart.store(reg::cr2, usart.load(reg::cr2)
			| cr2::stop_bits(stop));
	usart.store(reg::cr3, usart.load(reg::cr3)
			| cr3::crs(false) | cr3::rts(false));
	usart.store(reg::cr1, usart.load(reg::cr1)
			| cr1::usart_enable(true));
}

void send(int nth, uint8_t value) {
	auto &usart = usart_space[nth - 1];

	while (!(usart.load(reg::status) & status::transmit_empty));

	usart.store(reg::data, value);
}

void send(int nth, const void *data, size_t size) {
	const uint8_t *c = static_cast<const uint8_t *>(data);

	for (size_t i = 0; i < size; i++, c++)
		send(nth, *c);
}

} // namespace platform::usart

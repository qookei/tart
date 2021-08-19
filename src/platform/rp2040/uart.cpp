#include "uart.hpp"
#include "uart_reg.hpp"

#include "resets.hpp"

namespace platform::usart {

void init(int nth, int baud) {
	auto &usart = usart_space[nth - 1];
	constexpr uint32_t clock = 125000000;

	switch (nth) {
		case 1: resets::reset(resets::periph::uart0); break;
		case 2: resets::reset(resets::periph::uart1); break;
	}

	auto iobank0 = (volatile uint32_t *)0x40014000;
	iobank0[0 * 2 + 1] = (2 << 0);

	usart.store(reg::control, control::uart_en(false));

	uint64_t int_part = clock / (16 * baud);

	// 3 decimal places of precision should be enough :^)
	uint64_t frac_part = (((clock * 1000) / (16 * baud) - (int_part * 1000))
		* 64 + 500) / 1000;

	usart.store(reg::i_baud, int_part);
	usart.store(reg::f_baud, frac_part);

	// 8n1, fifo enabled
	usart.store(reg::line_control,
			line_control::word_len(3)
			| line_control::fifo_en(true));
	usart.store(reg::control,
			control::rx_en(true)
			| control::tx_en(true)
			| control::uart_en(true));
}

void send(int nth, uint8_t value) {
	auto &usart = usart_space[nth - 1];

	while (usart.load(reg::status) & status::tx_full)
		;

	usart.store(reg::data, value);
}

void send(int nth, const void *data, size_t size) {
	const uint8_t *c = static_cast<const uint8_t *>(data);

	for (size_t i = 0; i < size; i++, c++)
		send(nth, *c);
}

} // namespace platform::usart

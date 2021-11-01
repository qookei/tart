#include <tart/chip/rp2040/uart.hpp>
#include <tart/hw/rp2040/uart.hpp>

#include <tart/chip/rp2040/resets.hpp>

#include <tart/chip/uart.hpp>

namespace platform::usart {

using namespace tart::hw::uart;

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

namespace tart::chip {

using namespace platform;
using namespace tart::hw::uart;

void uart::init(int baud) {
	constexpr uint32_t clock = 125000000;

	switch (nth_) {
		case 0: resets::reset(resets::periph::uart0); break;
		case 1: resets::reset(resets::periph::uart1); break;
	}

	// TODO: this is specific to uart0
	auto iobank0 = (volatile uint32_t *)0x40014000;
	iobank0[0 * 2 + 1] = (2 << 0);

	space_.store(reg::control, control::uart_en(false));

	uint64_t int_part = clock / (16 * baud);

	uint64_t frac_part = (((clock * 1000) / (16 * baud) - (int_part * 1000))
		* 64 + 500) / 1000;

	space_.store(reg::i_baud, int_part);
	space_.store(reg::f_baud, frac_part);

	// 8n1, fifo enabled
	space_.store(reg::line_control,
			line_control::word_len(3)
			| line_control::fifo_en(true));
	space_.store(reg::control,
			control::rx_en(true)
			| control::tx_en(true)
			| control::uart_en(true));
}

void uart::send_sync(const void *data, size_t size) {
	const uint8_t *c = static_cast<const uint8_t *>(data);

	for (size_t i = 0; i < size; i++, c++) {
		while (tx_full())
			;

		space_.store(reg::data, *c);
	}
}

void uart::send_sync(const char *data) {
	while (*data) {
		while (tx_full())
			;

		space_.store(reg::data, *data++);
	}
}

bool uart::tx_full() const {
	return space_.load(reg::status) & status::tx_full;
}

bool uart::rx_empty() const {
	return space_.load(reg::status) & status::rx_empty;
}

} // namespace tart::chip

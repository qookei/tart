#include <tart/chip/rp2040/uart.hpp>
#include <tart/hw/rp2040/uart.hpp>

#include <tart/chip/rp2040/resets.hpp>

#include <tart/chip/uart.hpp>

#include <tart/log.hpp>

#include <tart/irq_service.hpp>

#include <tart/chip/rp2040/hw.hpp>

namespace tart::chip {

using namespace platform;
using namespace tart::hw::uart;

void uart::init(int baud) {
	constexpr uint32_t clock = 125000000;

	switch (nth_) {
		case 0: resets::reset(resets::periph::uart0); break;
		case 1: resets::reset(resets::periph::uart1); break;
	}

	gpio->configure(0, 2, gpio_flags::output);
	gpio->configure(1, 2, gpio_flags::input | gpio_flags::pull_up);

	space_.store(reg::control, control::uart_en(false));

	uint64_t int_part = clock / (16 * baud);

	uint64_t frac_part = (((clock * 1000) / (16 * baud) - (int_part * 1000))
		* 64 + 500) / 1000;

	space_.store(reg::i_baud, int_part);
	space_.store(reg::f_baud, frac_part);

	space_.store(reg::irq_mask, irqs::tx(false) | irqs::rx(true));
	space_.store(reg::irq_clear, irqs::tx(true) | irqs::rx(true));

	auto nvic_iser = (volatile uint32_t *)0xe000e100;
	*nvic_iser = 1 << 20;

	// 8n1, fifo enabled
	space_.store(reg::line_control,
			line_control::word_len(3)
			| line_control::fifo_en(false));
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

size_t uart::recv_sync(void *data, size_t max_size) {
	char *buf = reinterpret_cast<char *>(data);
	size_t i = 0;

	while (!rx_empty() && i < max_size) {
		buf[i++] = space_.load(reg::data);
	}

	return i;
}

bool uart::tx_full() const {
	return space_.load(reg::status) & status::tx_full;
}

bool uart::rx_empty() const {
	return space_.load(reg::status) & status::rx_empty;
}

void uart::handle_irq() {
	auto status = space_.load(reg::irq_status);

	if (status & irqs::rx) {
		if (!pending_rx_.empty()) {
			auto node = pending_rx_.pop_front();
			irq_w.post(node);
		}

	}

	space_.store(reg::irq_clear, status);
}

} // namespace tart::chip

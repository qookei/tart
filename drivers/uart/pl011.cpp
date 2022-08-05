#include <tart/drivers/uart/pl011.hpp>

namespace {

namespace reg {
	inline constexpr arch::scalar_register<uint32_t> data{0x00};
	inline constexpr arch::bit_register<uint32_t> status{0x18};
	inline constexpr arch::scalar_register<uint32_t> i_baud{0x24};
	inline constexpr arch::scalar_register<uint32_t> f_baud{0x28};
	inline constexpr arch::bit_register<uint32_t> control{0x30};
	inline constexpr arch::bit_register<uint32_t> line_control{0x2c};
	inline constexpr arch::bit_register<uint32_t> irq_mask{0x38};
	inline constexpr arch::bit_register<uint32_t> irq_clear{0x44};
	inline constexpr arch::bit_register<uint32_t> irq_status{0x40};
} // namespace reg

namespace status {
	inline constexpr arch::field<uint32_t, bool> tx_full{5, 1};
	inline constexpr arch::field<uint32_t, bool> rx_empty{4, 1};
} // namespace status

namespace control {
	inline constexpr arch::field<uint32_t, bool> rx_en{9, 1};
	inline constexpr arch::field<uint32_t, bool> tx_en{8, 1};
	inline constexpr arch::field<uint32_t, bool> uart_en{0, 1};
} // namespace control

namespace line_control {
	inline constexpr arch::field<uint32_t, uint8_t> word_len{5, 2};
	inline constexpr arch::field<uint32_t, bool> fifo_en{4, 1};
} // namespace line_control

namespace irqs {
	inline constexpr arch::field<uint32_t, bool> overrun_err{10, 1};
	inline constexpr arch::field<uint32_t, bool> break_err{9, 1};
	inline constexpr arch::field<uint32_t, bool> parity_err{8, 1};
	inline constexpr arch::field<uint32_t, bool> frame_err{7, 1};
	inline constexpr arch::field<uint32_t, bool> rx_timeout{6, 1};
	inline constexpr arch::field<uint32_t, bool> tx{5, 1};
	inline constexpr arch::field<uint32_t, bool> rx{4, 1};
} // namespace irq_mask

} // namespace anonymous

namespace tart {

void pl011_uart::configure(uart_params params) {
	// TODO(qookie): Use given clock once it's there
	constexpr uint32_t clock = 125000000;

	if (reset_)
		reset_->reset();

	if (gpios_.tx)
		gpios_.tx->configure();

	if (gpios_.rx)
		gpios_.rx->configure();

	space_.store(reg::control, control::uart_en(false));

	uint64_t int_part = clock / (16 * params.baud_rate);

	uint64_t frac_part = (((clock * 1000) / (16 * params.baud_rate) - (int_part * 1000))
		* 64 + 500) / 1000;

	space_.store(reg::i_baud, int_part);
	space_.store(reg::f_baud, frac_part);

	space_.store(reg::irq_mask, irqs::tx(false) | irqs::rx(false));
	space_.store(reg::irq_clear, irqs::tx(true) | irqs::rx(true));

	// TODO(qookie): uart irqs
	//auto nvic_iser = (volatile uint32_t *)0xe000e100;
	//*nvic_iser = 1 << 20;

	// 8n1, fifo enabled
	space_.store(reg::line_control,
			line_control::word_len(3)
			| line_control::fifo_en(true));
	space_.store(reg::control,
			control::rx_en(gpios_.rx)
			| control::tx_en(gpios_.tx)
			| control::uart_en(true));
}

size_t pl011_uart::try_write(const void *data, size_t size) {
	const uint8_t *buf = static_cast<const uint8_t *>(data);
	size_t i = 0;

	while (!tx_full() && i < size) {
		space_.store(reg::data, buf[i++]);
	}

	return i;
}

size_t pl011_uart::try_read(void *data, size_t size) {
	char *buf = reinterpret_cast<char *>(data);
	size_t i = 0;

	while (!rx_empty() && i < size) {
		buf[i++] = space_.load(reg::data);
	}

	return i;
}

bool pl011_uart::tx_full() const {
	return space_.load(reg::status) & status::tx_full;
}

bool pl011_uart::rx_empty() const {
	return space_.load(reg::status) & status::rx_empty;
}

} // namespace tart

#pragma once

#include <tart/drivers/gpio.hpp>
#include <tart/log.hpp>

#include <stdint.h>
#include <stddef.h>

namespace tart {
	struct uart_gpios {
		// Base pins
		const gpio_pin *rx;
		const gpio_pin *tx;

		// Hardware flow control (optional)
		const gpio_pin *rts = nullptr;
		const gpio_pin *cts = nullptr;
	};

	// TODO(qookie): Add fields for data bits, stop bits, and parity.
	struct uart_params {
		unsigned int baud_rate;
	};

	static inline constexpr uart_params b115200_8n1 = {115200};

	struct uart : log_sink {
		constexpr uart(uart_gpios gpios)
		: gpios_{gpios} { }

		virtual void configure(uart_params params) = 0;
		virtual size_t try_write(const void *data, size_t size) = 0;
		virtual size_t try_read(void *data, size_t size) = 0;

		void blocking_write(const void *data, size_t size) {
			auto cur = reinterpret_cast<const char *>(data);
			size_t progress = 0;

			while (progress < size) {
				auto written = try_write(cur, size - progress);
				progress += written;
				cur += written;
			}
		}

		void blocking_write(const char *msg) override {
			blocking_write(msg, strlen(msg));
		}

	protected:
		~uart() = default;
		uart_gpios gpios_;
	};

} // namespace tart

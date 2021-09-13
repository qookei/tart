#pragma once

#include <stdint.h>

namespace chip {
	struct uart {
		uart(int nth)
		: nth_{nth} { }

		void init(uint32_t baud);
		void deinit();

		void send(uint8_t v);
		void send(const void *data, size_t size);

		uint8_t recv_blocking();
	private:
		int nth_;
	};
} // namespace chip

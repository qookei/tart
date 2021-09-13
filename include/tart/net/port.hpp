#pragma once

#include <stdint.h>

namespace net {
	struct port_allocator {
		static constexpr uint16_t n_ports = 4096;
		static constexpr uint16_t n_bytes = n_ports / 8;
		static constexpr uint16_t base = 65535 - 4096;

		port_allocator()
		: bitmap_{} { }

		uint16_t allocate() {
			for (uint16_t i = 0; i < n_ports; i++) {
				if (!(bitmap_[i / 8] & (1 << (i % 8)))) {
					bitmap_[i / 8] |= (1 << (i % 8));
					return i + base;
				}
			}

			assert(!"out of ports...");
		}

		void free(uint16_t port) {
			assert(port >= base);
			uint16_t p = port - base;

			assert(bitmap_[p / 8] & (1 << (p % 8)));
			bitmap_[p / 8] &= ~(1 << (p % 8));
		}

	private:
		uint8_t bitmap_[n_bytes];
	};
} // namespace net

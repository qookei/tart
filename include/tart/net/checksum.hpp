#pragma once

#include <stdint.h>
#include <stddef.h>

namespace net {
	struct checksum {
		void update(uint16_t n) {
			v_ += n;

			while (v_ >> 16) {
				v_ = (v_ >> 16) + (v_ & 0xFFFF);
			}
		}

		void update(const void *buf, size_t size) {
			auto b = static_cast<const uint8_t *>(buf);

			if (size & 1) {
				size--;
				update(uint16_t(b[size]) << 8);
			}

			size_t i = 0;
			while (i < size) {
				update((uint16_t(b[i]) << 8) | b[i + 1]);
				i += 2;
			}
		}

		uint16_t value() {
			return ~v_;
		}

		void reset() {
			v_ = 0;
		}

		uint16_t operator()(const void *buf, size_t size) {
			update(buf, size);
			return value();
		}

		static uint16_t compute(const void *buf, size_t size) {
			checksum cs;
			return cs(buf, size);
		}
	private:
		uint32_t v_ = 0;
	};
} // namespace net

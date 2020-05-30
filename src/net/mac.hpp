#pragma once

#include <stdint.h>
#include <stddef.h>

namespace net {
	struct mac {
		uint8_t v[6];
		uint8_t operator[](size_t i) const {
			return v[i];
		}

		bool operator==(const mac &other) const {
			return v[0] == other[0]
				&& v[1] == other[1]
				&& v[2] == other[2]
				&& v[3] == other[3]
				&& v[4] == other[4]
				&& v[5] == other[5];
		}

		bool operator!=(const mac &other) const {
			return !(*this == other);
		}

		operator bool() const {
			return *this != mac{0, 0, 0, 0, 0, 0};
		}

		static mac from_bytes(const void *data) {
			const uint8_t *bytes = static_cast<const uint8_t *>(data);
			return mac{bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]};
		}

		size_t get_size() {
			return 6;
		}

		void *to_bytes(void *ptr) {
			uint8_t *dest = static_cast<uint8_t *>(ptr);
			for (int i = 0; i < 6; i++)
				*dest++ = v[i];

			return dest;
		}
	};

	namespace mac_addr {
		static constexpr mac broadcast = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	} // namespace mac
} // namespace net

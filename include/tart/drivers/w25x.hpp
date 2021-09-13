#pragma once

#include <tart/periph/spi.hpp>
#include <stddef.h>

namespace drivers {
	// Driver for W25X10/20/40/80 series flash chips
	// TODO: add erase and program support
	struct w25x_flash {
		w25x_flash(spi::spi_dev *dev)
		:dev_{dev} {}

		void read(void *buffer, uint32_t addr, size_t size);
		void vendor_info(uint8_t &vendor, uint8_t &device);
	private:
		spi::spi_dev *dev_;
	};
} // namespace drivers

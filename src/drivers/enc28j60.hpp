#pragma once

#include <periph/spi.hpp>

// TODO: move to it's own header
namespace net {
	struct mac {
		uint8_t v[6];
		uint8_t operator[](size_t i) const {
			return v[i];
		}
	};
} // namespace net

namespace drivers {
	struct enc28j60_nic {
		enc28j60_nic(spi::spi_dev *dev)
		:dev_{dev} {}

		void setup(const net::mac &mac);
		void run();
	private:
		void set_bank(uint8_t bank);
		uint8_t read_reg(uint8_t reg);
		void write_reg(uint8_t reg, uint8_t val);

		uint8_t read_reg_raw(uint8_t reg, bool needs_dummy);
		void write_reg_raw(uint8_t reg, uint8_t val);

		uint16_t read_phy(uint8_t reg);
		void write_phy(uint8_t reg, uint16_t val);

		void read_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr = false);
		void write_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr = false);
		void reset();

		spi::spi_dev *dev_;
	};

} // namespace drivers

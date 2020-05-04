#pragma once

#include <periph/spi.hpp>
#include <net/process.hpp>
#include <net/mac.hpp>
#include <async/result.hpp>

namespace drivers {
	struct enc28j60_nic {
		enc28j60_nic(spi::spi_dev *dev)
		:dev_{dev} {}

		async::detached run(const net::mac &mac, net::processor &pr);
	private:
		async::result<void> setup(const net::mac &mac);

		async::result<void> set_bank(uint8_t bank);
		async::result<uint8_t> read_reg(uint8_t reg);
		async::result<void> write_reg(uint8_t reg, uint8_t val);
		async::result<void> reg_bit_set(uint8_t reg, uint8_t val);
		async::result<void> reg_bit_reset(uint8_t reg, uint8_t val);

		async::result<uint8_t> read_reg_raw(uint8_t reg, bool needs_dummy);
		async::result<void> write_reg_raw(uint8_t reg, uint8_t val);

		async::result<uint16_t> read_phy(uint8_t reg);
		async::result<void> write_phy(uint8_t reg, uint16_t val);

		async::result<void> read_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr = false);
		async::result<void> write_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr = false);
		async::result<void> reset();

		spi::spi_dev *dev_;
	};

} // namespace drivers

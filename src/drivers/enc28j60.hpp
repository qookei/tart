#pragma once

#include <periph/spi.hpp>
#include <net/process.hpp>
#include <net/mac.hpp>
#include <async/result.hpp>
#include <async/doorbell.hpp>
#include <async/service.hpp>

namespace drivers {
	struct enc28j60_nic : public service::pollable {
		enc28j60_nic(spi::spi_dev *dev)
		:dev_{dev}, send_queue_{mem::get_allocator()} {}

		void setup(const net::mac &mac);
		async::detached run(net::processor &pr);
		bool do_poll() override;
	private:
		async::detached run_send();

		void set_bank(uint8_t bank);
		uint8_t read_reg(uint8_t reg);
		void write_reg(uint8_t reg, uint8_t val);
		void reg_bit_set(uint8_t reg, uint8_t val);
		void reg_bit_reset(uint8_t reg, uint8_t val);

		uint8_t read_reg_raw(uint8_t reg, bool needs_dummy);
		void write_reg_raw(uint8_t reg, uint8_t val);

		uint16_t read_phy(uint8_t reg);
		void write_phy(uint8_t reg, uint16_t val);

		void read_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr = false);
		void write_buffer(void *src, uint16_t addr, size_t size, bool set_ptr = false);
		void reset();

		spi::spi_dev *dev_;
		async::queue<mem::buffer, mem::allocator> send_queue_;

		async::doorbell transmit_irq_;
		std::atomic_bool transmit_error_;

		async::doorbell receive_irq_;
		std::atomic_bool receive_error_;

		async::doorbell link_irq_;
	};

} // namespace drivers

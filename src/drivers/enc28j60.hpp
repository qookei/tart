#pragma once

#include <periph/spi.hpp>
#include <net/dispatch.hpp>
#include <net/address.hpp>
#include <async/result.hpp>
#include <async/recurring-event.hpp>
#include <async/queue.hpp>
#include <async/mutex.hpp>
#include <async/service.hpp>

namespace drivers {
	struct enc28j60_nic : public service::pollable {
		enc28j60_nic(spi::spi_dev *dev)
		:dev_{dev}, send_queue_{mem::get_allocator()}, recv_queue_{mem::get_allocator()} {}

		void setup(const net::mac_addr &mac);
		async::detached run();
		bool do_poll() override;

		net::mac_addr mac() {
			return mac_;
		}

		async::result<void> send_packet(mem::buffer &&b);

		async::result<mem::buffer> recv_packet() {
			co_return std::move(*(co_await recv_queue_.async_get()));
		}
	private:
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
		async::mutex send_mutex_;
		async::queue<mem::buffer, mem::allocator> send_queue_;
		async::queue<mem::buffer, mem::allocator> recv_queue_;

		async::recurring_event transmit_irq_;
		std::atomic_bool transmit_error_;

		async::recurring_event receive_irq_;
		std::atomic_bool receive_error_;

		async::recurring_event link_irq_;

		net::mac_addr mac_;
	};

	static_assert(net::nic<enc28j60_nic>);
} // namespace drivers

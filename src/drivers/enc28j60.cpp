#include <periph/transmit.hpp>
#include "enc28j60.hpp"
#define ENC28J60_PRIVATE
#include "enc28j60_reg.hpp"

#include <lib/logger.hpp>
#include <mem/buffer.hpp>
#include <frg/utility.hpp>
#include <net/process.hpp>

namespace drivers {

namespace {
	constexpr uint16_t rx_start = 0x1000;
	constexpr uint16_t rx_end = 0x1FFF;
}

async::result<void> enc28j60_nic::setup(const net::mac &mac) {
	lib::log("enc28j60_nic::setup: resetting\r\n");
	co_await reset();
	for (int i = 0; i < 8000000; i++) asm volatile ("nop");
	lib::log("enc28j60_nic::setup: setting up\r\n");

	// program receive buffer
	// recv buffer at 0x1000 - 0x1FFF
	// program start pointer
	co_await write_reg(reg::erxstl, rx_start & 0xFF);
	co_await write_reg(reg::erxsth, (rx_start >> 8) & 0xFF);
	// program end pointer
	co_await write_reg(reg::erxndl, rx_end & 0xFF);
	co_await write_reg(reg::erxndh, (rx_end >> 8) & 0xFF);
	// program receive read pointer
	co_await write_reg(reg::erxrdptl, rx_end & 0xFF);
	co_await write_reg(reg::erxrdpth, (rx_end >> 8) & 0xFF);

	// wait for ost
	while(!((co_await read_reg(reg::estat)) & estat::clkrdy));

	// setup mac
	// enable receive and ieee flow control
	co_await write_reg(reg::macon1, macon1::marxen | macon1::txpaus | macon1::rxpaus);
	// enable full duplex and automatic padding and automatic crc
	co_await write_reg(reg::macon3, macon3::fuldpx | macon3::txcrcen | macon3::padcfg_101);
	co_await write_reg(reg::macon4, 0);

	// program back-to-back inter-packet gap delay
	co_await write_reg(reg::mabbipg, 0x15); // ieee minimum of 9.6us

	// program local mac address
	co_await write_reg(reg::maadr1, mac[0]);
	co_await write_reg(reg::maadr2, mac[1]);
	co_await write_reg(reg::maadr3, mac[2]);
	co_await write_reg(reg::maadr4, mac[3]);
	co_await write_reg(reg::maadr5, mac[4]);
	co_await write_reg(reg::maadr6, mac[5]);

	// setup phy
	co_await write_phy(phy_reg::phcon1, phcon1::pdpxmd);
}

async::detached enc28j60_nic::run(const net::mac &mac, net::processor &pr) {
	co_await setup(mac);

	uint8_t rev = co_await read_reg(reg::erevid);
	lib::log("enc28j60_nic::run: sillicon revision: %02x\r\n", rev);

	co_await reg_bit_set(reg::econ1, econ1::rxen);

	uint16_t cur_ptr = rx_start;

	lib::log("enc28j60_nic::run: polling for packet recv\r\n");
	while(1) {
		while (!(co_await read_reg(reg::epktcnt)))
			co_await async::yield_to_current_queue();

		lib::log("enc28j60_nic::run: received packet(s)\r\n");

		while (co_await read_reg(reg::epktcnt)) {
			uint16_t next_ptr;
			uint8_t status[4];
			co_await read_buffer(&next_ptr, cur_ptr, 2, true);
			co_await read_buffer(status, 0, 4);
			uint16_t len = frg::min(
				uint16_t(status[0]) | (uint16_t(status[1]) << 8),
				1522);

			cur_ptr = next_ptr;

			mem::buffer buf{len};
			co_await read_buffer(buf.data(), 0, len);

			if (next_ptr == rx_start) {
				co_await write_reg(reg::erxrdptl, rx_end & 0xFF);
				co_await write_reg(reg::erxrdpth, (rx_end >> 8) & 0xFF);
			} else {
				co_await write_reg(reg::erxrdptl, (next_ptr - 1) & 0xFF);
				co_await write_reg(reg::erxrdpth, ((next_ptr - 1) >> 8) & 0xFF);
			}

			pr.push_packet(std::move(buf));
			co_await reg_bit_set(reg::econ2, econ2::pktdec);
		}
	}
}

async::result<void> enc28j60_nic::set_bank(uint8_t bank) {
	uint8_t v = co_await read_reg_raw(reg::econ1, false);
	v &= ~0b11;
	v |= bank & 0b11;
	co_await write_reg_raw(reg::econ1, v);
}

static inline bool is_mac_reg(uint8_t r) {
	return (r >= reg_no(2, 0) && r <= reg_no(2, 0x1A))
		|| (r >= reg_no(3, 0) && r <= reg_no(3, 0x0A));
}

async::result<uint8_t> enc28j60_nic::read_reg(uint8_t r) {
	co_await set_bank(r >> 5);
	co_return co_await read_reg_raw(r & 0b11111, is_mac_reg(r));
}

async::result<void> enc28j60_nic::write_reg(uint8_t r, uint8_t v) {
	co_await set_bank(r >> 5);
	co_await write_reg_raw(r & 0b11111, v);
}

async::result<void> enc28j60_nic::reg_bit_set(uint8_t r, uint8_t val) {
	co_await set_bank(r >> 5);

	if (is_mac_reg(r)) {
		// do load, modify, store
		uint8_t v = co_await read_reg_raw(r & 0b11111, true);
		co_await write_reg_raw(r & 0b11111, v | val);
		co_return;
	}

	co_await transmit::do_transmission(dev_,
		transmit::send_bytes(
			(spi_cmd::bit_set << 5) | (r & 0b11111),
			val
		)
	);
}

async::result<void> enc28j60_nic::reg_bit_reset(uint8_t r, uint8_t val) {
	co_await set_bank(r >> 5);

	if (is_mac_reg(r)) {
		// do load, modify, store
		uint8_t v = co_await read_reg_raw(r & 0b11111, true);
		co_await write_reg_raw(r & 0b11111, v & ~val);
		co_return;
	}

	co_await transmit::do_transmission(dev_,
		transmit::send_bytes(
			(spi_cmd::bit_clear << 5) | (r & 0b11111),
			val
		)
	);
}

async::result<uint8_t> enc28j60_nic::read_reg_raw(uint8_t r, bool needs_dummy) {
	uint8_t v[2];
	co_await transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::read_control << 5) | r),
		transmit::recv_buffer(v, needs_dummy ? 2 : 1)
	);

	co_return v[needs_dummy ? 1 : 0];
}

async::result<void> enc28j60_nic::write_reg_raw(uint8_t r, uint8_t v) {
	co_await transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::write_control << 5) | r),
		transmit::send_bytes(v)
	);
}

async::result<uint16_t> enc28j60_nic::read_phy(uint8_t reg) {
	co_await write_reg(reg::miregadr, reg);

	co_await reg_bit_set(reg::micmd, micmd::miird);
	while ((co_await read_reg(reg::mistat)) & mistat::busy);
	co_await reg_bit_reset(reg::micmd, micmd::miird);

	co_return (uint16_t(co_await read_reg(reg::mirdh)) << 8)
		| uint16_t(co_await read_reg(reg::mirdl));
}

async::result<void> enc28j60_nic::write_phy(uint8_t reg, uint16_t val) {
	co_await write_reg(reg::miregadr, reg);

	co_await write_reg(reg::miwrl, val & 0xFF);
	co_await write_reg(reg::miwrh, (val >> 8) & 0xFF);

	while ((co_await read_reg(reg::mistat)) & mistat::busy);
}

async::result<void> enc28j60_nic::read_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr) {
	if (set_ptr) {
		co_await write_reg(reg::erdptl, addr & 0xFF);
		co_await write_reg(reg::erdpth, (addr >> 8) & 0xFF);
	}

	co_await transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::read_buffer << 5) | 0x1A),
		transmit::recv_buffer(reinterpret_cast<uint8_t *>(dest), size)
	);
}

async::result<void> enc28j60_nic::write_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr) {
	if (set_ptr) {
		co_await write_reg(reg::ewrptl, addr & 0xFF);
		co_await write_reg(reg::ewrpth, (addr >> 8) & 0xFF);
	}

	co_await transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::write_buffer << 5) | 0x1A),
		transmit::send_buffer(reinterpret_cast<uint8_t *>(dest), size)
	);
}

async::result<void> enc28j60_nic::reset() {
	co_await transmit::do_transmission(dev_, transmit::send_bytes(0xFF));
}

} // namespace drivers

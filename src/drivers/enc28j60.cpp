#include <periph/transmit.hpp>
#include "enc28j60.hpp"
#define ENC28J60_PRIVATE
#include "enc28j60_reg.hpp"

#include <lib/logger.hpp>

#include <frg/utility.hpp>

#include <net/process.hpp>

namespace drivers {

namespace {
	constexpr uint16_t rx_start = 0x1000;
	constexpr uint16_t rx_end = 0x1FFF;
}

void enc28j60_nic::setup(const net::mac &mac) {
	lib::log("enc28j60_nic::setup: resetting\r\n");
	reset();
	for (int i = 0; i < 8000000; i++) asm volatile ("nop");
	lib::log("enc28j60_nic::setup: setting up\r\n");

	// program receive buffer
	// recv buffer at 0x1000 - 0x1FFF
	// program start pointer
	write_reg(reg::erxstl, rx_start & 0xFF);
	write_reg(reg::erxsth, (rx_start >> 8) & 0xFF);
	// program end pointer
	write_reg(reg::erxndl, rx_end & 0xFF);
	write_reg(reg::erxndh, (rx_end >> 8) & 0xFF);
	// program receive read pointer
	write_reg(reg::erxrdptl, rx_end & 0xFF);
	write_reg(reg::erxrdpth, (rx_end >> 8) & 0xFF);

	// wait for ost
	while(!(read_reg(reg::estat) & estat::clkrdy));

	// setup mac
	// enable receive and ieee flow control
	write_reg(reg::macon1, macon1::marxen | macon1::txpaus | macon1::rxpaus);
	// enable full duplex and automatic padding and automatic crc
	write_reg(reg::macon3, macon3::fuldpx | macon3::txcrcen | macon3::padcfg_101);
	write_reg(reg::macon4, 0);

	// program back-to-back inter-packet gap delay
	write_reg(reg::mabbipg, 0x15); // ieee minimum of 9.6us

	// program local mac address
	write_reg(reg::maadr1, mac[0]);
	write_reg(reg::maadr2, mac[1]);
	write_reg(reg::maadr3, mac[2]);
	write_reg(reg::maadr4, mac[3]);
	write_reg(reg::maadr5, mac[4]);
	write_reg(reg::maadr6, mac[5]);

	// setup phy
	write_phy(phy_reg::phcon1, phcon1::pdpxmd);
}

async::detached enc28j60_nic::run() {
	uint8_t rev = read_reg(reg::erevid);
	lib::log("enc28j60_nic::run: sillicon revision: %02x\r\n", rev);

	write_reg(reg::econ1, read_reg(reg::econ1) | econ1::rxen);

	uint16_t cur_ptr = rx_start;

	lib::log("enc28j60_nic::run: polling for packet recv\r\n");
	while(1) {
		uint8_t old_packet_count = read_reg(reg::epktcnt);
		while (read_reg(reg::epktcnt) == old_packet_count);
		write_reg(reg::epktcnt, 0);

		lib::log("enc28j60_nic::run: received packet\r\n");

		uint16_t next_ptr;
		uint8_t status[4];
		read_buffer(&next_ptr, cur_ptr, 2, true);
		read_buffer(status, 0, 4);
		uint16_t len = uint16_t(status[0]) | (uint16_t(status[1]) << 8);

		cur_ptr = next_ptr;

		uint8_t buf[2048];
		read_buffer(buf, 0, frg::min<uint16_t>(len, 2048));

		if (next_ptr == rx_start) {
			write_reg(reg::erxrdptl, rx_end & 0xFF);
			write_reg(reg::erxrdpth, (rx_end >> 8) & 0xFF);
		} else {
			write_reg(reg::erxrdptl, (next_ptr - 1) & 0xFF);
			write_reg(reg::erxrdpth, ((next_ptr - 1) >> 8) & 0xFF);
		}

		net::process_packet(buf, frg::min<uint16_t>(len, 2048));

		co_await async::yield_to_current_queue();
	}
}

void enc28j60_nic::set_bank(uint8_t bank) {
	uint8_t v = read_reg_raw(reg::econ1, false);
	v &= ~0b11;
	v |= bank & 0b11;
	write_reg_raw(reg::econ1, v);
}

static inline bool is_mac_reg(uint8_t r) {
	return (r >= reg_no(2, 0) && r <= reg_no(2, 0x1A))
		|| (r >= reg_no(3, 0) && r <= reg_no(3, 0x0A));
}

uint8_t enc28j60_nic::read_reg(uint8_t r) {
	set_bank(r >> 5);
	return read_reg_raw(r & 0b11111, is_mac_reg(r));
}

void enc28j60_nic::write_reg(uint8_t r, uint8_t v) {
	set_bank(r >> 5);
	write_reg_raw(r & 0b11111, v);
}

uint8_t enc28j60_nic::read_reg_raw(uint8_t r, bool needs_dummy) {
	uint8_t v[2];
	transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::read_control << 5) | r),
		transmit::recv_buffer(v, needs_dummy ? 2 : 1)
	);

	return v[needs_dummy ? 1 : 0];
}

void enc28j60_nic::write_reg_raw(uint8_t r, uint8_t v) {
	transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::write_control << 5) | r),
		transmit::send_bytes(v)
	);
}

uint16_t enc28j60_nic::read_phy(uint8_t reg) {
	write_reg(reg::miregadr, reg);

	write_reg(reg::micmd, read_reg(reg::micmd) | micmd::miird);
	while (read_reg(reg::mistat) & mistat::busy);
	write_reg(reg::micmd, read_reg(reg::micmd) & ~micmd::miird);

	return (uint16_t(read_reg(reg::mirdh)) << 8) | uint16_t(read_reg(reg::mirdl));
}

void enc28j60_nic::write_phy(uint8_t reg, uint16_t val) {
	write_reg(reg::miregadr, reg);

	write_reg(reg::miwrl, val & 0xFF);
	write_reg(reg::miwrh, (val >> 8) & 0xFF);

	while (read_reg(reg::mistat) & mistat::busy);
}

void enc28j60_nic::read_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr) {
	if (set_ptr) {
		write_reg(reg::erdptl, addr & 0xFF);
		write_reg(reg::erdpth, (addr >> 8) & 0xFF);
	}

	transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::read_buffer << 5) | 0x1A),
		transmit::recv_buffer(reinterpret_cast<uint8_t *>(dest), size)
	);
}

void enc28j60_nic::write_buffer(void *dest, uint16_t addr, size_t size, bool set_ptr) {
	if (set_ptr) {
		write_reg(reg::ewrptl, addr & 0xFF);
		write_reg(reg::ewrpth, (addr >> 8) & 0xFF);
	}

	transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::write_buffer << 5) | 0x1A),
		transmit::send_buffer(reinterpret_cast<uint8_t *>(dest), size)
	);
}

void enc28j60_nic::reset() {
	transmit::do_transmission(dev_, transmit::send_bytes(0xFF));
}

} // namespace drivers

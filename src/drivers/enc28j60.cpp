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

	constexpr uint16_t tx_start = 0x0000;
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

	// program non-back-to-back inter-packet gap delay
	write_reg(reg::mapipgl, 0x12);

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

async::detached enc28j60_nic::run(net::processor &pr) {

	uint8_t rev = read_reg(reg::erevid);
	lib::log("enc28j60_nic::run: sillicon revision: %02x\r\n", rev);

	reg_bit_set(reg::econ1, econ1::rxen);

	uint16_t cur_ptr = rx_start;

	uint8_t test_data[] = {
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x56, 0x95, 0x77, 0x9C, 0x48, 0xBA,
		0x08, 0x06,
		0x00, 0x01,
		0x08, 0x00,
		0x06,
		0x04,
		0x00, 0x01,
		0x56, 0x95, 0x77, 0x9C, 0x48, 0xBA,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xC0, 0xA8, 0x01, 0x66
	};

	mem::buffer pkt{sizeof(test_data)};
	memcpy(pkt.data(), test_data, pkt.size());

	lib::log("enc28j60_nic: submitting test packet send (arp query for 192.168.1.102)\r\n");
	send_queue_.emplace(std::move(pkt));

	run_send();

	lib::log("enc28j60_nic::run: polling for packet recv\r\n");
	while(1) {
		while (!(read_reg(reg::epktcnt)))
			co_await async::yield_to_current_queue();

		lib::log("enc28j60_nic::run: received %u packet(s)\r\n", read_reg(reg::epktcnt));

		while (read_reg(reg::epktcnt)) {
			uint16_t next_ptr;
			uint8_t status[4];
			read_buffer(&next_ptr, cur_ptr, 2, true);
			read_buffer(status, 0, 4);
			uint16_t len = frg::min(
				uint16_t(status[0]) | (uint16_t(status[1]) << 8),
				1522);

			cur_ptr = next_ptr;

			mem::buffer buf{len};
			read_buffer(buf.data(), 0, len);

			if (next_ptr == rx_start) {
				write_reg(reg::erxrdptl, rx_end & 0xFF);
				write_reg(reg::erxrdpth, (rx_end >> 8) & 0xFF);
			} else {
				write_reg(reg::erxrdptl, (next_ptr - 1) & 0xFF);
				write_reg(reg::erxrdpth, ((next_ptr - 1) >> 8) & 0xFF);
			}

			pr.push_packet(std::move(buf));
			reg_bit_set(reg::econ2, econ2::pktdec);
		}
	}
}

async::detached enc28j60_nic::run_send() {
	while(true) {
		auto buffer = std::move(*(co_await send_queue_.async_get()));
		lib::log("enc28j60_nic::run_send(): got a packet to send\r\n");

		reg_bit_set(reg::econ1, econ1::txrst);
		reg_bit_reset(reg::econ1, econ1::txrst);
		reg_bit_reset(reg::eir, eir::txif | eir::txerif);

		// the end pointer points at the last byte, not after it
		uint16_t tx_end = tx_start + buffer.size();
		// program start pointer
		write_reg(reg::etxstl, tx_start & 0xFF);
		write_reg(reg::etxsth, (tx_start >> 8) & 0xFF);

		uint8_t control[1] = {0}; // use settings from MACON3
		// write data
		write_buffer(control, tx_start, 1, true);
		write_buffer(buffer.data(), 0, buffer.size());

		// program end pointer
		write_reg(reg::etxndl, tx_end & 0xFF);
		write_reg(reg::etxndh, (tx_end >> 8) & 0xFF);

		// trigger send
		reg_bit_set(reg::econ1, econ1::txrts);

		constexpr int timeout = 450000;
		int it = 0;
		while (!(read_reg(reg::eir) & (eir::txif | eir::txerif))
				&& it++ < timeout)
			co_await async::yield_to_current_queue();

		reg_bit_reset(reg::econ1, econ1::txrts);

		bool tx_done = read_reg(reg::eir) & (eir::txif | eir::txerif);

		if (it >= timeout && tx_done) {
			lib::log("enc28j60_nic::run_send: transmission timed out\r\n");
			continue;
		}

		auto r = read_reg(reg::estat);
		if (r & estat::txabrt) {
			lib::log("enc28j60_nic::run_send: packet transmission aborted\r\n");
			if (r & estat::latecol) // TODO: unreliable
				lib::log("enc28j60_nic::run_send: aborted due to late collision\r\n");
		} else {
			lib::log("enc28j60_nic::run_send: packet transmitted successfully\r\n");
		}
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

void enc28j60_nic::reg_bit_set(uint8_t r, uint8_t val) {
	set_bank(r >> 5);

	if (is_mac_reg(r)) {
		// do load, modify, store
		uint8_t v = read_reg_raw(r & 0b11111, true);
		write_reg_raw(r & 0b11111, v | val);
		return;
	}

	transmit::do_transmission(dev_,
		transmit::send_bytes(
			(spi_cmd::bit_set << 5) | (r & 0b11111),
			val
		)
	);
}

void enc28j60_nic::reg_bit_reset(uint8_t r, uint8_t val) {
	set_bank(r >> 5);

	if (is_mac_reg(r)) {
		// do load, modify, store
		uint8_t v = read_reg_raw(r & 0b11111, true);
		write_reg_raw(r & 0b11111, v & ~val);
		return;
	}

	transmit::do_transmission(dev_,
		transmit::send_bytes(
			(spi_cmd::bit_clear << 5) | (r & 0b11111),
			val
		)
	);
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

	reg_bit_set(reg::micmd, micmd::miird);
	while (read_reg(reg::mistat) & mistat::busy);
	reg_bit_reset(reg::micmd, micmd::miird);

	return (uint16_t(read_reg(reg::mirdh)) << 8)
		| uint16_t(read_reg(reg::mirdl));
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

void enc28j60_nic::write_buffer(void *src, uint16_t addr, size_t size, bool set_ptr) {
	if (set_ptr) {
		write_reg(reg::ewrptl, addr & 0xFF);
		write_reg(reg::ewrpth, (addr >> 8) & 0xFF);
	}

	transmit::do_transmission(dev_,
		transmit::send_bytes((spi_cmd::write_buffer << 5) | 0x1A),
		transmit::send_buffer(reinterpret_cast<uint8_t *>(src), size)
	);
}

void enc28j60_nic::reset() {
	transmit::do_transmission(dev_, transmit::send_bytes(0xFF));
}

} // namespace drivers
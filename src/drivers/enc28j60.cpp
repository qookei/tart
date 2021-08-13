#include <periph/transmit.hpp>
#include "enc28j60.hpp"
#define ENC28J60_PRIVATE
#include "enc28j60_reg.hpp"

#include <lib/logger.hpp>
#include <mem/buffer.hpp>
#include <frg/utility.hpp>

namespace drivers {

namespace {
	constexpr uint16_t rx_start = 0x0000;
	constexpr uint16_t rx_end = 0x0FFF;

	constexpr uint16_t tx_start = 0x1000;
}

void enc28j60_nic::setup(const net::mac_addr &mac) {
	lib::log("enc28j60_nic::setup: resetting\r\n");
	reset();
	for (int i = 0; i < 20000000; i++) asm volatile ("nop");

	lib::log("enc28j60_nic::setup: waiting for ost\r\n");
	// wait for ost
	while(!(read_reg(reg::estat) & estat::clkrdy));

	lib::log("enc28j60_nic::setup: setting up\r\n");

	// program receive buffer
	// recv buffer at 0x0000 - 0x0FFF
	// program start pointer
	write_reg(reg::erxstl, rx_start & 0xFF);
	write_reg(reg::erxsth, (rx_start >> 8) & 0xFF);
	// program end pointer
	write_reg(reg::erxndl, rx_end & 0xFF);
	write_reg(reg::erxndh, (rx_end >> 8) & 0xFF);
	// program receive read pointer
	write_reg(reg::erxrdptl, rx_end & 0xFF);
	write_reg(reg::erxrdpth, (rx_end >> 8) & 0xFF);

	// setup mac
	// enable receive and ieee flow control

	write_reg(reg::macon1, macon1::marxen | macon1::txpaus | macon1::rxpaus);

	// enable full duplex and automatic padding and automatic crc
	write_reg(reg::macon3, macon3::fuldpx | macon3::txcrcen | macon3::padcfg_000);
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

	mac_ = mac;

	// enable reception
	reg_bit_set(reg::econ1, econ1::rxen);

	auto rev_to_str = [](uint8_t rev) -> const char * {
		switch (rev) {
			case 0b0000'0000: return "unknown (zero)";
			case 0b0000'0010: return "B1";
			case 0b0000'0100: return "B4";
			case 0b0000'0101: return "B5";
			case 0b0000'0110: return "B7";
			default: return "unknown (other)";
		}
	};

	lib::log("enc28j60_nic::run: sillicon revision: %s\r\n",
			rev_to_str(read_reg(reg::erevid)));
}

#ifdef ENC28J60_VERBOSE
static inline void dump_tsv(uint8_t *tsv) {
	uint16_t tx_bytes = uint16_t(tsv[0]) | uint16_t(tsv[1] << 8);
	uint16_t total_bytes = uint16_t(tsv[4]) | uint16_t(tsv[5] << 8);
	uint8_t coll_count = tsv[2] & 0xF;
	lib::log("enc28j60_nic::send_packet: tx bytes = %lu\r\n", tx_bytes);
	lib::log("enc28j60_nic::send_packet: total bytes on the wire = %lu\r\n", total_bytes);
	lib::log("enc28j60_nic::send_packet: collisions = %lu\r\n", coll_count);
	if (tsv[2] & (1 << 4))
		lib::log("enc28j60_nic::send_packet: crc error\r\n");

	if (tsv[2] & (1 << 5))
		lib::log("enc28j60_nic::send_packet: length check error\r\n");

	if (tsv[2] & (1 << 6))
		lib::log("enc28j60_nic::send_packet: length out of range\r\n");

	if (tsv[2] & (1 << 7))
		lib::log("enc28j60_nic::send_packet: completed\r\n");

	if (tsv[3] & (1 << 0))
		lib::log("enc28j60_nic::send_packet: multicast\r\n");

	if (tsv[3] & (1 << 1))
		lib::log("enc28j60_nic::send_packet: broadcast\r\n");

	if (tsv[3] & (1 << 2))
		lib::log("enc28j60_nic::send_packet: packet defer\r\n");

	if (tsv[3] & (1 << 3))
		lib::log("enc28j60_nic::send_packet: excessive defer\r\n");

	if (tsv[3] & (1 << 4))
		lib::log("enc28j60_nic::send_packet: excessive collision\r\n");

	if (tsv[3] & (1 << 5))
		lib::log("enc28j60_nic::send_packet: late collision\r\n");

	if (tsv[3] & (1 << 6))
		lib::log("enc28j60_nic::send_packet: giant\r\n");

	if (tsv[3] & (1 << 7))
		lib::log("enc28j60_nic::send_packet: underrun\r\n");

	if (tsv[6] & (1 << 0))
		lib::log("enc28j60_nic::send_packet: control frame\r\n");

	if (tsv[6] & (1 << 1))
		lib::log("enc28j60_nic::send_packet: pause control frame\r\n");

	if (tsv[6] & (1 << 2))
		lib::log("enc28j60_nic::send_packet: backpressure applied\r\n");

	if (tsv[6] & (1 << 3))
		lib::log("enc28j60_nic::send_packet: vlan tagged frame\r\n");
}
#endif

async::detached enc28j60_nic::run() {
	uint16_t cur_ptr = rx_start;

	while (true) {
		co_await receive_irq_.async_wait();
		bool was_err = receive_error_.exchange(false);

		if (was_err) {
#ifdef ENC28J60_TELL_ME_MORE
			lib::log("enc28j60_nic::run: an receive error occured? ...\r\n");
			lib::log("enc28j60_nic::run: epktcnt = %u\r\n", read_reg(reg::epktcnt));

			lib::log("enc28j60_nic::run: cur_ptr = %04x\r\n", cur_ptr);

			uint8_t erxrdptl, erxrdpth;
			erxrdpth = read_reg(reg::erxrdpth);
			erxrdptl = read_reg(reg::erxrdptl);

			lib::log("enc28j60_nic::run: erxrdpt = %02x%02x\r\n", erxrdpth, erxrdptl);
			lib::log("enc28j70_nic::run: dazed and confused, but trying to continue...\r\n");
#endif
			continue;
		}

#ifdef ENC28J60_VERBOSE
		lib::log("enc28j60_nic::run: got 1 packet\r\n");
#endif

		uint16_t next_ptr;
		uint16_t len;
		uint8_t status[4];
		read_buffer(&next_ptr, cur_ptr, 2, true);
		read_buffer(status, 0, 4);
		memcpy(&len, status, 2);
		assert(len < 1522);

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

		recv_queue_.emplace(std::move(buf));
		reg_bit_set(reg::econ2, econ2::pktdec);

		// don't fill up the recv_queue_ too much and run out of memory by accident
		co_await async::yield_to_current_queue();
	}
}

async::result<void> enc28j60_nic::send_packet(mem::buffer &&buffer) {
	co_await send_mutex_.async_lock();

#ifdef ENC28J60_VERBOSE
	lib::log("enc28j60_nic::send_packet: got a packet to send\r\n");
#endif

	reg_bit_set(reg::econ1, econ1::txrst);
	reg_bit_reset(reg::econ1, econ1::txrst);
	reg_bit_reset(reg::eir, eir::txif | eir::txerif);

	constexpr size_t padding = 128;

	// the end pointer points at the last byte, not after it
	uint16_t tx_end = tx_start + frg::max(buffer.size(), padding);
	// program start pointer
	write_reg(reg::etxstl, tx_start & 0xFF);
	write_reg(reg::etxsth, (tx_start >> 8) & 0xFF);

	uint8_t control[1] = {0}; // use settings from MACON3
	// write data
	write_buffer(control, tx_start, 1, true);
	write_buffer(buffer.data(), 0, buffer.size());

	// pad buffer out with zeroes
	if (buffer.size() < padding) {
		size_t size = padding - buffer.size();
		uint8_t zeroes[padding];
		memset(zeroes, 0, size);
		write_buffer(zeroes, 0, size);
	}

	// program end pointer
	write_reg(reg::etxndl, tx_end & 0xFF);
	write_reg(reg::etxndh, (tx_end >> 8) & 0xFF);

	// trigger send
	reg_bit_set(reg::econ1, econ1::txrts);

	// wait for completion
	co_await transmit_irq_.async_wait();
	bool was_err = transmit_error_.exchange(false);
	assert(!was_err && "TODO: retry transmission");

	// read tsv
	uint8_t tsv[7];
	read_buffer(tsv, tx_end + 1, 7, true);

#ifdef ENC28J60_VERBOSE
	dump_tsv(tsv);

	auto r = read_reg(reg::estat);
	if (r & estat::txabrt) {
		lib::log("enc28j60_nic::send_packet: packet transmission aborted\r\n");
	} else {
		lib::log("enc28j60_nic::send_packet: packet transmitted successfully\r\n");
	}
#endif

	assert(tsv[2] & (1 << 7));

	send_mutex_.unlock();
}

bool enc28j60_nic::do_poll() {
	uint8_t eir = read_reg(reg::eir);
	uint8_t epktcnt = read_reg(reg::epktcnt);

	if (epktcnt || (eir & eir::pktif) || (eir & eir::rxerif)) {
		receive_irq_.raise();
		receive_error_ = eir & eir::rxerif;

		if (eir & eir::rxerif)
			reg_bit_reset(reg::eir, eir::rxerif);
	}

	if ((eir & eir::txif) || (eir & eir::txerif)) {
		transmit_irq_.raise();
		transmit_error_ = eir & eir::txerif;

		if (eir & eir::txif)
			reg_bit_reset(reg::eir, eir::txif);
		if (eir & eir::txerif)
			reg_bit_reset(reg::eir, eir::txerif);
	}

	if (eir & eir::linkif)
		link_irq_.raise();

	return epktcnt || eir;
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

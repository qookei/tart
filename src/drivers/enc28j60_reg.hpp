#pragma once

#ifndef ENC28J60_PRIVATE
#error This header is private to enc28j60.cpp
#endif

namespace spi_cmd {
	constexpr uint8_t read_control = 0;
	constexpr uint8_t read_buffer = 1;
	constexpr uint8_t write_control = 2;
	constexpr uint8_t write_buffer = 3;
	constexpr uint8_t bit_set = 4;
	constexpr uint8_t bit_clear = 5;
	constexpr uint8_t reset = 7;
} // namespace spi_cmd

static inline constexpr uint8_t reg_no(uint8_t bank, uint8_t reg) {
	return ((bank & 0b11) << 5) | (reg & 0b11111);
}

namespace reg {
	// all banks
	constexpr uint8_t eie = reg_no(0, 0x1B);
	constexpr uint8_t eir = reg_no(0, 0x1C);
	constexpr uint8_t estat = reg_no(0, 0x1D);
	constexpr uint8_t econ2 = reg_no(0, 0x1E);
	constexpr uint8_t econ1 = reg_no(0, 0x1F);

	// bank 0
	constexpr uint8_t erdptl = reg_no(0, 0x0);
	constexpr uint8_t erdpth = reg_no(0, 0x1);
	constexpr uint8_t ewrptl = reg_no(0, 0x2);
	constexpr uint8_t ewrpth = reg_no(0, 0x3);
	constexpr uint8_t etxstl = reg_no(0, 0x4);
	constexpr uint8_t etxsth = reg_no(0, 0x5);
	constexpr uint8_t etxndl = reg_no(0, 0x6);
	constexpr uint8_t etxndh = reg_no(0, 0x7);
	constexpr uint8_t erxstl = reg_no(0, 0x8);
	constexpr uint8_t erxsth = reg_no(0, 0x9);
	constexpr uint8_t erxndl = reg_no(0, 0xA);
	constexpr uint8_t erxndh = reg_no(0, 0xB);
	constexpr uint8_t erxrdptl = reg_no(0, 0xC);
	constexpr uint8_t erxrdpth = reg_no(0, 0xD);
	constexpr uint8_t erxwrptl = reg_no(0, 0xE);
	constexpr uint8_t erxwrpth = reg_no(0, 0xF);

	// bank 1
	constexpr uint8_t epktcnt = reg_no(1, 0x19);

	// bank 2
	constexpr uint8_t macon1 = reg_no(2, 0x0);
	constexpr uint8_t macon3 = reg_no(2, 0x2);
	constexpr uint8_t macon4 = reg_no(2, 0x3);
	constexpr uint8_t mabbipg = reg_no(2, 0x4);
	constexpr uint8_t mapipgl = reg_no(2, 0x6);
	constexpr uint8_t micmd = reg_no(2, 0x12);
	constexpr uint8_t miregadr = reg_no(2, 0x14);
	constexpr uint8_t miwrl = reg_no(2, 0x16);
	constexpr uint8_t miwrh = reg_no(2, 0x17);
	constexpr uint8_t mirdl = reg_no(2, 0x18);
	constexpr uint8_t mirdh = reg_no(2, 0x19);

	// bank 3
	constexpr uint8_t maadr5 = reg_no(3, 0x0);
	constexpr uint8_t maadr6 = reg_no(3, 0x1);
	constexpr uint8_t maadr3 = reg_no(3, 0x2);
	constexpr uint8_t maadr4 = reg_no(3, 0x3);
	constexpr uint8_t maadr1 = reg_no(3, 0x4);
	constexpr uint8_t maadr2 = reg_no(3, 0x5);
	constexpr uint8_t mistat = reg_no(3, 0xA);
	constexpr uint8_t erevid = reg_no(3, 0x12);
} // namespace reg

namespace phy_reg {
	constexpr uint8_t phcon1 = 0x0;
	constexpr uint8_t phstat1 = 0x1;
	constexpr uint8_t phid1 = 0x2;
	constexpr uint8_t phid2 = 0x3;
	constexpr uint8_t phcon2 = 0x10;
	constexpr uint8_t phstat2 = 0x11;
	constexpr uint8_t phie = 0x12;
	constexpr uint8_t phir = 0x13;
	constexpr uint8_t phlcon = 0x14;
} // namespace phy_reg

namespace phcon1 {
	constexpr uint16_t pdpxmd = (1 << 8);
} // namespace phcon1

namespace estat {
	constexpr uint8_t clkrdy = (1 << 0);
	constexpr uint8_t txabrt = (1 << 1);
	constexpr uint8_t latecol = (1 << 4);
} // namespace estat

namespace econ1 {
	constexpr uint8_t rxen = (1 << 2);
	constexpr uint8_t txrts = (1 << 3);
	constexpr uint8_t txrst = (1 << 7);
} // namespace econ1

namespace econ2 {
	constexpr uint8_t autoinc = (1 << 7);
	constexpr uint8_t pktdec = (1 << 6);
} // namespace econ2

namespace eir {
	constexpr uint8_t pktif = (1 << 6);
	constexpr uint8_t txif = (1 << 3);
	constexpr uint8_t txerif = (1 << 1);
} // namespace eir

namespace macon1 {
	constexpr uint8_t txpaus = (1 << 3);
	constexpr uint8_t rxpaus = (1 << 2);
	constexpr uint8_t pasall = (1 << 1);
	constexpr uint8_t marxen = (1 << 0);
} // namespace macon1

namespace macon3 {
	constexpr uint8_t txcrcen = (1 << 4);
	constexpr uint8_t phdren = (1 << 3);
	constexpr uint8_t hfrmen = (1 << 2);
	constexpr uint8_t frmlnen = (1 << 1);
	constexpr uint8_t fuldpx = (1 << 0);
	constexpr uint8_t padcfg_101 = (0b101 << 5);
} // namespace macon3

namespace mistat {
	constexpr uint8_t busy = (1 << 0);
} // namespace mistat

namespace micmd {
	constexpr uint8_t miird = (1 << 0);
	constexpr uint8_t miiscan = (1 << 0);
} // namespace mistat

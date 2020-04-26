#include "../platform.hpp"
#include "rcc.hpp"
#include "usart.hpp"
#include "gpio.hpp"
#include "spi.hpp"

#include <lib/logger.hpp>
#include <lib/string.hpp>

namespace mem {
void test();
}

namespace platform {

void setup() {
	rcc::clock_setup_ext_8mhz_72mhz();
	usart::init(1, 115200, usart::parity::none, usart::stop_bits::one);
	spi::init(1, 8, 0, false, spi::size::eight);
}

void nmi() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

[[gnu::used]]
extern "C" void actual_hard_fault(uintptr_t *stack) {
	lib::log("unexpected hard fault\r\n");
	lib::log("r0  = 0x%08x\r\n", stack[0]);
	lib::log("r1  = 0x%08x\r\n", stack[1]);
	lib::log("r2  = 0x%08x\r\n", stack[2]);
	lib::log("r3  = 0x%08x\r\n", stack[3]);
	lib::log("r12 = 0x%08x\r\n", stack[4]);
	lib::log("lr  = 0x%08x\r\n", stack[5]);
	lib::log("pc  = 0x%08x\r\n", stack[6]);
	lib::log("psr = 0x%08x\r\n", stack[7]);

	while(1);
}

[[gnu::naked]]
void hard_fault() {
	asm volatile("mov r0, sp; b actual_hard_fault" : : : "r0", "memory");
}

void mm_fault() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

void bus_fault() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

void usage_fault() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

void sv_call() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

void pend_sv_call() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

void systick() {
	usart::send(1, "unexpected exception\r\n", 22);
	while(1);
}

static void spi_vendor(uint8_t &manufacturer, uint8_t &device) {
	gpio::set_pin(gpio::bank::a, 3, false);

	spi::transfer(1, 0x90);
	spi::transfer(1, 0x00);
	spi::transfer(1, 0x00);
	spi::transfer(1, 0x00);

	manufacturer = spi::transfer(1, 0);
	device = spi::transfer(1, 0);

	gpio::set_pin(gpio::bank::a, 3, true);
}

static void spi_read(uint8_t *buffer, uint32_t address, size_t size) {
	gpio::set_pin(gpio::bank::a, 3, false);
	spi::transfer(1, 0x03);
	spi::transfer(1, (address >> 16) & 0xFF);
	spi::transfer(1, (address >> 8)  & 0xFF);
	spi::transfer(1, (address)       & 0xFF);

	for (size_t i = 0; i < size; i++)
		*buffer++ = spi::transfer(1, 0);

	gpio::set_pin(gpio::bank::a, 3, true);
}

constexpr size_t bytes_per_line = 16;

#define min(x, y) ((x) > (y) ? (y) : (x))

void buffer_pretty_print(const void *buf, size_t size) {
	uintptr_t addr = reinterpret_cast<uintptr_t>(buf);

	for (size_t i = 0; i < (size + bytes_per_line - 1) / bytes_per_line; i++) {
		uint8_t buf[bytes_per_line];
		auto off = i * bytes_per_line;
		size_t n = min(bytes_per_line, size - off);
		memcpy(
			buf,
			reinterpret_cast<const void *>(addr + off),
			n
		);

		lib::log(" %08lx: ", addr + off);

		for (size_t j = 0; j < n; j++)
			lib::log("%02x ", buf[j]);
		for (size_t j = 0; j < bytes_per_line - n; j++)
			lib::log("   ");

		for (auto &c : buf)
			c = (c >= ' ' && c <= '~') ? c : '.';

		lib::log("| %.*s\r\n", static_cast<int>(n), buf);
	}
}

void run() {
	gpio::setup_pin(gpio::bank::a, 3, gpio::mode::output_50mhz, gpio::conf::push_pull);
	gpio::setup_pin(gpio::bank::b, 12, gpio::mode::output_50mhz, gpio::conf::push_pull);
	gpio::set_pin(gpio::bank::a, 3, true);
	gpio::set_pin(gpio::bank::b, 12, true);
	lib::log("tart: hello!\r\n");
	lib::log("tart: running frg::slab_pool test code!\r\n");
	mem::test();

	uint8_t buf[128];
	lib::log("tart: trying out SPI1!\r\n");

	uint8_t m, d;
	spi_vendor(m, d);
	lib::log("tart: manufacturer = %02x, device = %02x\r\n", m, d);

	lib::log("tart: reading 128 bytes:\r\n");
	spi_read(buf, 0, 128);
	buffer_pretty_print(buf, 128);

	while(1);
}

} // namespace platform

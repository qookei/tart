#include "../platform.hpp"
#include "rcc.hpp"
#include "usart.hpp"
#include <periph/spi.hpp>

#include <lib/logger.hpp>
#include <lib/string.hpp>

namespace platform {

void setup() {
	rcc::clock_setup_ext_8mhz_72mhz();
	usart::init(1, 115200, usart::parity::none, usart::stop_bits::one);
	spi::get_spi(1)->setup(64);
	spi::get_spi(2)->setup(64);
}

void nmi() {
	lib::log("unexpected nmi\r\n");
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
	lib::log("unexpected mm fault\r\n");
	while(1);
}

void bus_fault() {
	lib::log("unexpected bus fault\r\n");
	while(1);
}

void usage_fault() {
	lib::log("unexpected usage fault\r\n");
	while(1);
}

void sv_call() {
	lib::log("unexpected sv call\r\n");
	while(1);
}

void pend_sv_call() {
	lib::log("unexpected pending sv call\r\n");
	while(1);
}

void systick() {
	lib::log("unexpected systick\r\n");
	while(1);
}

} // namespace platform

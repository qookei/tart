#include <tart/arch/platform.hpp>

#include <tart/chip/rp2040/uart.hpp>
#include <tart/chip/rp2040/resets.hpp>
#include <tart/chip/rp2040/clocks.hpp>

#include <tart/lib/logger.hpp>
#include <tart/lib/string.hpp>

#include <stdint.h>

namespace tart::chip {

using namespace platform;

void early_init() {
	clocks::setup_ext_12mhz();

	resets::reset(resets::periph::io_bank0);
	resets::reset(resets::periph::pads_bank0);

	auto iobank0 = (volatile uint32_t *)0x40014000;
	auto sio = (volatile uint32_t *)0xd0000000;

	iobank0[25 * 2 + 1] = (5 << 0);

	sio[8] = 1 << 25;
	sio[4] = 1 << 25;

	usart::init(1, 115200);
}

void nmi() {
	lib::panic("unexpected nmi\r\n");
}

extern "C" [[gnu::used]] void actual_hard_fault(uintptr_t *stack) {
	lib::log("unexpected hard fault\r\n");
	lib::log("r0  = 0x%08x\r\n", stack[0]);
	lib::log("r1  = 0x%08x\r\n", stack[1]);
	lib::log("r2  = 0x%08x\r\n", stack[2]);
	lib::log("r3  = 0x%08x\r\n", stack[3]);
	lib::log("r12 = 0x%08x\r\n", stack[4]);
	lib::log("lr  = 0x%08x\r\n", stack[5]);
	lib::log("pc  = 0x%08x\r\n", stack[6]);
	lib::log("psr = 0x%08x\r\n", stack[7]);

	lib::panic("unexpected hard fault\r\n");
}

[[gnu::naked]]
void hard_fault() {
	asm volatile("mov r0, sp; b actual_hard_fault" : : : "r0", "memory");
}

void mm_fault() {
	lib::panic("unexpected mm fault\r\n");
}

void bus_fault() {
	lib::panic("unexpected bus fault\r\n");
}

void usage_fault() {
	lib::panic("unexpected usage fault\r\n");
}

void sv_call() {
	lib::panic("unexpected sv call\r\n");
}

void pend_sv_call() {
	lib::panic("unexpected pending sv call\r\n");
}

void systick() {
	lib::panic("unexpected systick\r\n");
}

} // namespace tart::chip

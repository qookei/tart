#include "../platform.hpp"
#include "rcc.hpp"
#include "usart.hpp"
#include "gpio.hpp"

#include <lib/logger.hpp>

namespace mem {
void test();
}

namespace platform {

void setup() {
	rcc::clock_setup_ext_8mhz_72mhz();
	usart::init(1, 115200, usart::parity::none, usart::stop_bits::one);
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

void run() {
	gpio::setup_pin(gpio::bank::b, 4, gpio::mode::output_50mhz, gpio::conf::push_pull);
	for (int j = 0; j < 20; j++) {
		gpio::set_pin(gpio::bank::b, 4, j & 1);
		for (int i = 0; i < 800000; i++)
			asm volatile("nop");
	}
	lib::log("tart: hello!\r\n");
	lib::log("tart: running frg::slab_pool test code!\r\n");
	mem::test();

	while(1);
}

} // namespace platform

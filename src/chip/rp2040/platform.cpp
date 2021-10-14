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

} // namespace tart::chip
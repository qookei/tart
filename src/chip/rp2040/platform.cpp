#include <tart/chip/rp2040/resets.hpp>
#include <tart/chip/rp2040/clocks.hpp>

#include <stdint.h>

namespace tart::chip {

using namespace platform;

void init() {
	resets::reset(resets::periph::io_bank0);
	resets::reset(resets::periph::pads_bank0);
}

} // namespace tart::chip

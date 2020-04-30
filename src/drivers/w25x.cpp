#include "w25x.hpp"
#include <periph/transmit.hpp>

namespace drivers {

void w25x_flash::read(void *buffer, uint32_t addr, size_t size) {
	transmit::do_transmission(dev_,
		transmit::send_bytes(
			0x03,
			(addr >> 16) & 0xFF,
			(addr >> 8) & 0xFF,
			(addr) & 0xFF
		),
		transmit::recv_buffer(
			reinterpret_cast<uint8_t *>(buffer),
			size
		)
	);
}

void w25x_flash::vendor_info(uint8_t &vendor, uint8_t &device) {
	transmit::do_transmission(dev_,
		transmit::send_bytes(0x90, 0, 0, 0),
		transmit::recv_bytes(vendor, device)
	);
}

} // namespace drivers

#include <tart/arch/arm/systick.hpp>
#include <tart/arch/arm/systick_reg.hpp>

#include <tart/chip/irq.hpp>

namespace tart::arch {

void systick_init(uint32_t freq) {
	uint32_t reload = chip::get_core_freq() / freq;

	systick_space.store(systick::reg::rvr, reload - 1);
	systick_space.store(systick::reg::cvr, 0);
	systick_space.store(systick::reg::csr,
			systick::csr::enable(true)
			| systick::csr::tickint(true)
			| systick::csr::clksource(true));
}

} // namespace tart::arch

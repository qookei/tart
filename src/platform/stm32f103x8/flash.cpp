#include <tart/chip/stm32f103x8/flash.hpp>
#include <tart/chip/stm32f103x8/reg/flash.hpp>

void platform::flash::set_wait_states(int ws) {
	flash_cfg_space.store(reg::acr, (flash_cfg_space.load(reg::acr) & ~acr::ws) | acr::ws(ws));
}

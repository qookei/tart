#include "flash.hpp"
#include "flash_reg.hpp"

void platform::flash::set_wait_states(int ws) {
	flash_cfg_space.store(reg::acr, (flash_cfg_space.load(reg::acr) & ~acr::ws) | acr::ws(ws));
}

#pragma once

extern "C" void reset_handler();

namespace tart::arch {
	void early_init();
} // namespace tart::arch

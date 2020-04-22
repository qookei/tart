#pragma once

namespace platform {
	// vectors
	void entry();
	[[gnu::weak]] void nmi();
	[[gnu::weak]] void hard_fault();
	[[gnu::weak]] void mm_fault();
	[[gnu::weak]] void bus_fault();
	[[gnu::weak]] void usage_fault();
	[[gnu::weak]] void sv_call();
	[[gnu::weak]] void pend_sv_call();
	[[gnu::weak]] void systick();

	void setup();
	void run();
} // namespace platform

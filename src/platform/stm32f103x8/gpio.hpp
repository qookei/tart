#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/arm/mem_space.hpp>

namespace platform::gpio {
	static constexpr arch::mem_space gpio_space[5] = {
		0x40010800,
		0x40010C00,
		0x40011000,
		0x40011400,
		0x40011800
	};

	enum class mode : uint8_t {
		input = 0,
		output_10mhz,
		output_2mhz,
		output_50mhz
	};

	enum class conf : uint8_t {
		// input
		analog = 0,
		floating,
		pull_up_down,

		// output
		push_pull = 0,
		open_drain,
		af_push_pull,
		af_open_drain
	};

	enum class bank {
		a = 0,
		b,
		c,
		d,
		e
	};

	void setup_pin(bank b, int pin, mode m, conf c);
	void set_pin(bank b, int pin, bool high);
	bool get_pin(bank b, int pin);
} // platform::gpio

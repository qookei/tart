#pragma once

#include <stdint.h>

namespace gpio {
	enum class bank {
		a,b,c,d,e
	};

	struct pin {
		bank b;
		int p;
	};

	enum class mode : uint8_t {
		input,
		output_10mhz,
		output_2mhz,
		output_50mhz
	};

	enum class config : uint8_t {
		// input
		analog,
		floating,
		pull_up_down,

		// output
		push_pull = 0,
		open_drain
	};

	void setup(pin p, mode d, config c, bool alt_fn = false);
	void set(pin p, bool b);
	bool get(pin p);


	// This was generated with:
	//for b in "abcde":
	//	for i in range(16):
	//		print(f'\tstatic constexpr pin p{b}{i} = {{bank::{b}, {i}}};')
	static constexpr pin pa0 = {bank::a, 0};
	static constexpr pin pa1 = {bank::a, 1};
	static constexpr pin pa2 = {bank::a, 2};
	static constexpr pin pa3 = {bank::a, 3};
	static constexpr pin pa4 = {bank::a, 4};
	static constexpr pin pa5 = {bank::a, 5};
	static constexpr pin pa6 = {bank::a, 6};
	static constexpr pin pa7 = {bank::a, 7};
	static constexpr pin pa8 = {bank::a, 8};
	static constexpr pin pa9 = {bank::a, 9};
	static constexpr pin pa10 = {bank::a, 10};
	static constexpr pin pa11 = {bank::a, 11};
	static constexpr pin pa12 = {bank::a, 12};
	static constexpr pin pa13 = {bank::a, 13};
	static constexpr pin pa14 = {bank::a, 14};
	static constexpr pin pa15 = {bank::a, 15};
	static constexpr pin pb0 = {bank::b, 0};
	static constexpr pin pb1 = {bank::b, 1};
	static constexpr pin pb2 = {bank::b, 2};
	static constexpr pin pb3 = {bank::b, 3};
	static constexpr pin pb4 = {bank::b, 4};
	static constexpr pin pb5 = {bank::b, 5};
	static constexpr pin pb6 = {bank::b, 6};
	static constexpr pin pb7 = {bank::b, 7};
	static constexpr pin pb8 = {bank::b, 8};
	static constexpr pin pb9 = {bank::b, 9};
	static constexpr pin pb10 = {bank::b, 10};
	static constexpr pin pb11 = {bank::b, 11};
	static constexpr pin pb12 = {bank::b, 12};
	static constexpr pin pb13 = {bank::b, 13};
	static constexpr pin pb14 = {bank::b, 14};
	static constexpr pin pb15 = {bank::b, 15};
	static constexpr pin pc0 = {bank::c, 0};
	static constexpr pin pc1 = {bank::c, 1};
	static constexpr pin pc2 = {bank::c, 2};
	static constexpr pin pc3 = {bank::c, 3};
	static constexpr pin pc4 = {bank::c, 4};
	static constexpr pin pc5 = {bank::c, 5};
	static constexpr pin pc6 = {bank::c, 6};
	static constexpr pin pc7 = {bank::c, 7};
	static constexpr pin pc8 = {bank::c, 8};
	static constexpr pin pc9 = {bank::c, 9};
	static constexpr pin pc10 = {bank::c, 10};
	static constexpr pin pc11 = {bank::c, 11};
	static constexpr pin pc12 = {bank::c, 12};
	static constexpr pin pc13 = {bank::c, 13};
	static constexpr pin pc14 = {bank::c, 14};
	static constexpr pin pc15 = {bank::c, 15};
	static constexpr pin pd0 = {bank::d, 0};
	static constexpr pin pd1 = {bank::d, 1};
	static constexpr pin pd2 = {bank::d, 2};
	static constexpr pin pd3 = {bank::d, 3};
	static constexpr pin pd4 = {bank::d, 4};
	static constexpr pin pd5 = {bank::d, 5};
	static constexpr pin pd6 = {bank::d, 6};
	static constexpr pin pd7 = {bank::d, 7};
	static constexpr pin pd8 = {bank::d, 8};
	static constexpr pin pd9 = {bank::d, 9};
	static constexpr pin pd10 = {bank::d, 10};
	static constexpr pin pd11 = {bank::d, 11};
	static constexpr pin pd12 = {bank::d, 12};
	static constexpr pin pd13 = {bank::d, 13};
	static constexpr pin pd14 = {bank::d, 14};
	static constexpr pin pd15 = {bank::d, 15};
	static constexpr pin pe0 = {bank::e, 0};
	static constexpr pin pe1 = {bank::e, 1};
	static constexpr pin pe2 = {bank::e, 2};
	static constexpr pin pe3 = {bank::e, 3};
	static constexpr pin pe4 = {bank::e, 4};
	static constexpr pin pe5 = {bank::e, 5};
	static constexpr pin pe6 = {bank::e, 6};
	static constexpr pin pe7 = {bank::e, 7};
	static constexpr pin pe8 = {bank::e, 8};
	static constexpr pin pe9 = {bank::e, 9};
	static constexpr pin pe10 = {bank::e, 10};
	static constexpr pin pe11 = {bank::e, 11};
	static constexpr pin pe12 = {bank::e, 12};
	static constexpr pin pe13 = {bank::e, 13};
	static constexpr pin pe14 = {bank::e, 14};
	static constexpr pin pe15 = {bank::e, 15};
} // namespace gpio

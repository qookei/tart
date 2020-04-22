#pragma once

#include "rcc.hpp"

#include <stdint.h>
#include <stddef.h>

#include <arch/register.hpp>

namespace platform::rcc::reg {
	static constexpr arch::bit_register<uint32_t> cr{0x00};
	static constexpr arch::bit_register<uint32_t> cfgr{0x04};
	static constexpr arch::bit_register<uint32_t> cir{0x08};
	static constexpr arch::bit_register<uint32_t> apb2_rst{0x0C};
	static constexpr arch::bit_register<uint32_t> apb1_rst{0x10};
	static constexpr arch::bit_register<uint32_t> ahb_en{0x14};
	static constexpr arch::bit_register<uint32_t> apb2_en{0x18};
	static constexpr arch::bit_register<uint32_t> apb1_en{0x1C};
	static constexpr arch::bit_register<uint32_t> bdcr{0x20};
	static constexpr arch::bit_register<uint32_t> csr{0x24};
} // namespace platform::rcc::reg

namespace platform::rcc::cr {
	static constexpr arch::field<uint32_t, bool> pll_ready{25, 1};
	static constexpr arch::field<uint32_t, bool> pll_enable{24, 1};
	static constexpr arch::field<uint32_t, bool> css_enable{19, 1};
	static constexpr arch::field<uint32_t, bool> hse_bypass{18, 1};
	static constexpr arch::field<uint32_t, bool> hse_ready{17, 1};
	static constexpr arch::field<uint32_t, bool> hse_enable{16, 1};
	static constexpr arch::field<uint32_t, uint8_t> hsi_cal{8, 8};
	static constexpr arch::field<uint32_t, uint8_t> hsi_trim{3, 5};
	static constexpr arch::field<uint32_t, bool> hsi_ready{1, 1};
	static constexpr arch::field<uint32_t, bool> hsi_enable{0, 1};
} // namespace platform::rcc::cr

namespace platform::rcc::cfgr {
	enum class clock_source {
		hsi,
		hse,
		pll
	};

	static constexpr arch::field<uint32_t, uint8_t> mco{24, 3};
	static constexpr arch::field<uint32_t, bool> usb_pre{22, 1};
	static constexpr arch::field<uint32_t, uint8_t> pll_mul{18, 4};
	static constexpr arch::field<uint32_t, bool> pll_xt_pre{17, 1};
	static constexpr arch::field<uint32_t, bool> pll_src{16, 1};
	static constexpr arch::field<uint32_t, uint8_t> adc_pre{14, 2};
	static constexpr arch::field<uint32_t, uint8_t> apb2_pre{11, 3};
	static constexpr arch::field<uint32_t, uint8_t> apb1_pre{8, 3};
	static constexpr arch::field<uint32_t, uint8_t> ahb_pre{4, 4};
	static constexpr arch::field<uint32_t, uint8_t> sw_status{2, 2};
	static constexpr arch::field<uint32_t, clock_source> sw{0, 2};
} // namespace platform::rcc::cfgr

namespace platform::rcc::apb2_en {
	static constexpr arch::field<uint32_t, bool> usart1_enable{14, 1};
	static constexpr arch::field<uint32_t, bool> spi1_enable{12, 1};
	static constexpr arch::field<uint32_t, bool> gpioe_enable{6, 1};
	static constexpr arch::field<uint32_t, bool> gpiod_enable{5, 1};
	static constexpr arch::field<uint32_t, bool> gpioc_enable{4, 1};
	static constexpr arch::field<uint32_t, bool> gpiob_enable{3, 1};
	static constexpr arch::field<uint32_t, bool> gpioa_enable{2, 1};
	static constexpr arch::field<uint32_t, bool> afio_enable{0, 1};
} // namespace platform::rcc::apb2_en

namespace platform::rcc::apb1_en {
	static constexpr arch::field<uint32_t, bool> i2c2_enable{22, 1};
	static constexpr arch::field<uint32_t, bool> i2c1_enable{21, 1};
	static constexpr arch::field<uint32_t, bool> usart3_enable{18, 1};
	static constexpr arch::field<uint32_t, bool> usart2_enable{17, 1};
	static constexpr arch::field<uint32_t, bool> spi2_enable{14, 1};
} // namespace platform::rcc::apb1_en

namespace platform::rcc::ahb_en {
	static constexpr arch::field<uint32_t, bool> dma2_enable{1, 1};
	static constexpr arch::field<uint32_t, bool> dma1_enable{0, 1};
} // namespace platform::rcc::ahb_en

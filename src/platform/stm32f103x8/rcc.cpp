#include "rcc.hpp"
#include "rcc_reg.hpp"
#include "flash.hpp"

namespace platform::rcc {

enum class osc {
	hsi,
	hse,
	pll,
};

static void osc_on(osc o) {
	switch (o) {
		case osc::hsi:
			rcc_space.store(reg::cr, rcc_space.load(reg::cr) | cr::hsi_enable(true));
			break;
		case osc::hse:
			rcc_space.store(reg::cr, rcc_space.load(reg::cr) | cr::hse_enable(true));
			break;
		case osc::pll:
			rcc_space.store(reg::cr, rcc_space.load(reg::cr) | cr::pll_enable(true));
			break;
	}
}

void wait_osc_on(osc o) {
	switch (o) {
		case osc::hsi:
			while (!(rcc_space.load(reg::cr) & cr::hsi_ready));
			break;
		case osc::hse:
			while (!(rcc_space.load(reg::cr) & cr::hse_ready));
			break;
		case osc::pll:
			while (!(rcc_space.load(reg::cr) & cr::pll_ready));
			break;
	}
}

void set_sysclk_src(cfgr::clock_source c) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::sw) | cfgr::sw(c));
}

void set_ahb_prescaler(int pre) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::ahb_pre) | cfgr::ahb_pre(pre));
}

void set_apb1_prescaler(int pre) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::apb1_pre) | cfgr::apb1_pre(pre));
}

void set_apb2_prescaler(int pre) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::apb2_pre) | cfgr::apb2_pre(pre));
}

void set_adc_prescaler(int pre) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::adc_pre) | cfgr::adc_pre(pre));
}

void set_pll_mult_factor(int f) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::pll_mul) | cfgr::pll_mul(f - 2));
}

void set_pll_source(osc o) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::pll_src) | cfgr::pll_src(o == osc::hse));
}

void set_pll_xt_pre(bool divided) {
	rcc_space.store(reg::cfgr, (rcc_space.load(reg::cfgr) & ~cfgr::pll_xt_pre) | cfgr::pll_xt_pre(divided));
}

// TODO: generalize this
void clock_setup_ext_8mhz_72mhz() {
	osc_on(osc::hsi);
	wait_osc_on(osc::hsi);
	set_sysclk_src(cfgr::clock_source::hsi);

	osc_on(osc::hse);
	wait_osc_on(osc::hse);
	set_sysclk_src(cfgr::clock_source::hse);

	set_ahb_prescaler(0);
	set_adc_prescaler(3);
	set_apb1_prescaler(4);
	set_apb2_prescaler(0);

	flash::set_wait_states(2);

	set_pll_mult_factor(9);
	set_pll_source(osc::hse);
	set_pll_xt_pre(false);

	osc_on(osc::pll);
	wait_osc_on(osc::pll);
	set_sysclk_src(cfgr::clock_source::pll);
}

void set_periph_enable(periph p, bool enabled) {
	auto reg = reg::apb2_en;
	auto bit = apb2_en::afio_enable;

	switch(p) {
		case periph::afio: break;
		case periph::gpio_a: bit = apb2_en::gpioa_enable; break;
		case periph::gpio_b: bit = apb2_en::gpiob_enable; break;
		case periph::gpio_c: bit = apb2_en::gpioc_enable; break;
		case periph::gpio_d: bit = apb2_en::gpiod_enable; break;
		case periph::gpio_e: bit = apb2_en::gpioe_enable; break;
		case periph::spi1: bit = apb2_en::spi1_enable; break;
		case periph::usart1: bit = apb2_en::usart1_enable; break;

		case periph::spi2: bit = apb1_en::spi2_enable; reg = reg::apb1_en; break;
		case periph::i2c1: bit = apb1_en::i2c1_enable; reg = reg::apb1_en; break;
		case periph::i2c2: bit = apb1_en::i2c2_enable; reg = reg::apb1_en; break;
		case periph::usart2: bit = apb1_en::usart2_enable; reg = reg::apb1_en; break;
		case periph::usart3: bit = apb1_en::usart3_enable; reg = reg::apb1_en; break;

		case periph::dma1: bit = ahb_en::dma1_enable; reg = reg::ahb_en; break;
		case periph::dma2: bit = ahb_en::dma2_enable; reg = reg::ahb_en; break;
	}

	rcc_space.store(reg, (rcc_space.load(reg) & ~bit) | bit(enabled));
}

} // namespace platform::rcc

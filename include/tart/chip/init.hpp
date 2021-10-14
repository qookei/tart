#if defined(TART_CHIP_RP2040)
#include <tart/chip/rp2040/init.hpp>
#elif defined(TART_CHIP_STM32F103X8)
#include <tart/chip/stm32f103x8/init.hpp>
#else
#error Unknown chip
#endif

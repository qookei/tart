#include <tart/log.hpp>
// TODO add generic uart interface
#include <tart/chip/stm32f103x8/usart.hpp>

namespace tart {

void info_sink::operator()(const char *msg) {
	while (*msg)
		platform::usart::send(1, *msg++);
}

void debug_sink::operator()(const char *msg) {
	while (*msg)
		platform::usart::send(1, *msg++);
}

void fatal_sink::operator()(const char *msg) {
	while (*msg)
		platform::usart::send(1, *msg++);
}

void fatal_sink::finalize(bool) {
	// TODO: proper panic
	while(1);
}

} // namespace tart

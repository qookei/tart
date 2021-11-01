#include <tart/log.hpp>
#include <tart/chip/uart.hpp>

namespace tart {

inline chip::uart *log_sink = nullptr;

void info_sink::operator()(const char *msg) {
	if (log_sink)
		log_sink->send_sync(msg);
}

void debug_sink::operator()(const char *msg) {
	if (log_sink)
		log_sink->send_sync(msg);
}

void fatal_sink::operator()(const char *msg) {
	if (log_sink)
		log_sink->send_sync(msg);
}

void fatal_sink::finalize(bool) {
	// TODO: proper panic
	(*this)("\x1b[0m\r\n");
	while(1);
}

} // namespace tart

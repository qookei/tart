#include <tart/log.hpp>

namespace {
	tart::log_sink *sink = nullptr;
} // namespace anonymous

namespace tart {

void info_sink::operator()(const char *msg) {
	if (sink)
		sink->blocking_write(msg);
}

void debug_sink::operator()(const char *msg) {
	if (sink)
		sink->blocking_write(msg);
}

void fatal_sink::operator()(const char *msg) {
	if (sink)
		sink->blocking_write(msg);
}

void fatal_sink::finalize(bool) {
	// TODO: proper panic
	(*this)("\x1b[0m\r\n");
	while(1);
}

void set_log_sink(log_sink *sink) {
	::sink = sink;
}

} // namespace tart

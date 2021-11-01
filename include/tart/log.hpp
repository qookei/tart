#pragma once

#include <frg/logging.hpp>

namespace tart {

struct info_sink {
	constexpr info_sink() = default;
	void operator()(const char *msg);
	void finalize(bool) { (*this)("\r\n"); }
};

struct debug_sink {
	constexpr debug_sink() = default;
	void operator()(const char *msg);
	void begin() { (*this)("\x1b[33m"); }
	void finalize(bool) { (*this)("\x1b[0m\r\n"); }
};

struct fatal_sink {
	constexpr fatal_sink() = default;
	void operator()(const char *msg);
	void begin() { (*this)("\x1b[31m"); }
	void finalize(bool);
};

inline constinit frg::stack_buffer_logger<debug_sink> dbg;
inline constinit frg::stack_buffer_logger<info_sink> info;
inline constinit frg::stack_buffer_logger<fatal_sink> fatal;

} // namespace tart

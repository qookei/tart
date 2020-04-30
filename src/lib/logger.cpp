#include "logger.hpp"
#include <stdarg.h>

#include <frg/formatting.hpp>
#include <frg/utility.hpp>

// TODO: make a generic usart iface
#include <platform/stm32f103x8/usart.hpp>

namespace lib {

void log(const char *fmt, ...) {
	frg::va_struct vs;

	va_start(vs.args, fmt);

	struct {
		frg::va_struct *_vsp;

		void append(char c) {
			platform::usart::send(1, c);
		}

		void append(const char *s) {
			while (*s)
				platform::usart::send(1, *s++);
		}

		void operator()(char c) {
			platform::usart::send(1, c);
		}

		void operator()(const char *s, size_t n) {
			platform::usart::send(1, s, n);
		}

		void operator()(char t, frg::format_options opts, frg::printf_size_mod szmod) {
			switch(t) {
				case 'p': case 'c': case 's':
					frg::do_printf_chars(*this, t, opts, szmod, _vsp);
					break;
				case 'd': case 'i': case 'o': case 'x': case 'X': case 'u':
					frg::do_printf_ints(*this, t, opts, szmod, _vsp);
					break;
				case 'f': case 'F': case 'g': case 'G': case 'e': case 'E':
					frg::do_printf_floats(*this, t, opts, szmod, _vsp);
					break;
			}
		}
	} usart_formatter{&vs};

	frg::printf_format(usart_formatter, fmt, &vs);
	va_end(vs.args);
}

extern "C" void frg_panic(const char *msg) {
	log("frg panic: %s\r\n", msg);
	while(1);
}

extern "C" void frg_log(const char *msg) {
	log("frg log: %s\r\n", msg);
}

constexpr size_t bytes_per_line = 16;

void dump_buffer(const void *buf, size_t size, bool show_base) {
	uintptr_t addr = reinterpret_cast<uintptr_t>(buf);

	for (size_t i = 0; i < (size + bytes_per_line - 1) / bytes_per_line; i++) {
		uint8_t buf[bytes_per_line];
		auto off = i * bytes_per_line;
		size_t n = frg::min(bytes_per_line, size - off);
		memcpy(
			buf,
			reinterpret_cast<const void *>(addr + off),
			n
		);

		log(" %08lx: ", (show_base ? addr : 0) + off);

		for (size_t j = 0; j < n; j++)
			log("%02x ", buf[j]);
		for (size_t j = 0; j < bytes_per_line - n; j++)
			log("   ");

		for (auto &c : buf)
			c = (c >= ' ' && c <= '~') ? c : '.';

		log("| %.*s\r\n", static_cast<int>(n), buf);
	}
}

} // namespace lib

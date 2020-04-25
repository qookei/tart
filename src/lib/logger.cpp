#include "logger.hpp"
#include <stdarg.h>

#include <frg/formatting.hpp>

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

} // namespace lib

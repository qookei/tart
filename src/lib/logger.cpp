#include "logger.hpp"
#include <stdarg.h>

#include <frg/printf.hpp>
#include <frg/formatting.hpp>
#include <frg/utility.hpp>

// TODO: make a generic usart iface
#include <platform/stm32f103x8/usart.hpp>

#include <platform/stack.hpp>

namespace lib {

namespace {

void vlog(const char *fmt, va_list va) {
	frg::va_struct vs;

	va_copy(vs.args, va);

	struct {
		frg::va_struct *_vsp;

		void append(char c) {
			platform::usart::send(1, c);
		}

		void append(const char *s) {
			while (*s)
				platform::usart::send(1, *s++);
		}

		frg::expected<frg::format_error> operator()(char c) {
			platform::usart::send(1, c);
			return {};
		}

		frg::expected<frg::format_error> operator()(const char *s, size_t n) {
			platform::usart::send(1, s, n);
			return {};
		}

		frg::expected<frg::format_error> operator()(char t, frg::format_options opts, frg::printf_size_mod szmod) {
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

			return {};
		}
	} usart_formatter{&vs};

	frg::printf_format(usart_formatter, fmt, &vs).unwrap();
}

} // namespace anonymous

void log(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	vlog(fmt, va);
	va_end(va);
}

[[noreturn]] void panic(const char *fmt, ...) {
	log("PANIC!\r\n");

	va_list va;
	va_start(va, fmt);
	vlog(fmt, va);
	va_end(va);

	size_t n = 0;
	platform::walk_stack([&n](uintptr_t ptr) {
		lib::log("  #%lu -> %08lx\r\n", n, ptr);
		n++;
	});

	while(1);
	__builtin_unreachable();
}

extern "C" void frg_panic(const char *msg) {
	panic("frg panic: %s\r\n", msg);
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

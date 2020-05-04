#pragma once

#include <stddef.h>

namespace lib {

void log(const char *fmt, ...);
[[noreturn]] void panic(const char *fmt, ...);
void dump_buffer(const void *buf, size_t size, bool show_base = false);

}

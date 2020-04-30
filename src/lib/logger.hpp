#pragma once

namespace lib {

void log(const char *fmt, ...);
void dump_buffer(const void *buf, size_t size, bool show_base = false);

}

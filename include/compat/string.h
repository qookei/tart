#pragma once

#include <stddef.h>

extern "C" {
	void *memcpy(void *dest, const void *src, size_t size);
	void *memset(void *dest, int value, size_t size);
	void *memmove(void *dest, const void *src, size_t size);

	size_t strlen(const char *str);
}

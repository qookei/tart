#include "string.hpp"

void *memcpy(void *dest, const void *src, size_t size) {
	char *d = static_cast<char *>(dest);
	const char *s = static_cast<const char *>(src);

	while (size--)
		*d++ = *s++;

	return dest;
}

void *memset(void *dest, int value, size_t size) {
	char *d = static_cast<char *>(dest);

	while (size--)
		*d++ = value;

	return dest;
}

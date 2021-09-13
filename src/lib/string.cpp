#include <tart/lib/string.hpp>

[[gnu::used]] void *memcpy(void *dest, const void *src, size_t size) {
	char *d = static_cast<char *>(dest);
	const char *s = static_cast<const char *>(src);

	while (size--)
		*d++ = *s++;

	return dest;
}

[[gnu::used]] void *memset(void *dest, int value, size_t size) {
	char *d = static_cast<char *>(dest);

	while (size--)
		*d++ = value;

	return dest;
}

[[gnu::used]] void *memmove(void *dest, const void *src, size_t count) {
	auto d = static_cast<char *>(dest);
	auto s = static_cast<char const *>(src);

	if (s < d) {
		s += count;
		d += count;
		while (count--)
			*--d = *--s;
	} else {
		while (count--)
			*d++ = *s++;
	}

	return dest;
}

size_t strlen(const char *str) {
	size_t s = 0;

	while (*str++)
		s++;

	return s;
}

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

void *memmove(void *dest, const void *src, size_t count) {
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


extern "C" {
	[[gnu::used]] void *__aeabi_memclr(void *dest, size_t size) {
		return memset(dest, 0, size);
	}

	[[gnu::used]] void *__aeabi_memclr4(void *dest, size_t size) {
		return memset(dest, 0, size);
	}

	[[gnu::used]] void *__aeabi_memclr8(void *dest, size_t size) {
		return memset(dest, 0, size);
	}

	[[gnu::used]] void *__aeabi_memcpy(void *dest, const void *src, size_t size) {
		return memcpy(dest, src, size);
	}

	[[gnu::used]] void *__aeabi_memcpy4(void *dest, const void *src, size_t size) {
		return memcpy(dest, src, size);
	}

	[[gnu::used]] void *__aeabi_memmove4(void *dest, const void *src, size_t size) {
		return memmove(dest, src, size);
	}
}

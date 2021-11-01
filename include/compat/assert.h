#pragma once

#define assert(expr) \
	do { \
		if(!(expr)) \
			assert_failure__(#expr, __FILE__, __LINE__, __func__);\
	} while(0);

[[noreturn]] void assert_failure__(const char *expr, const char *file, int line, const char *func);

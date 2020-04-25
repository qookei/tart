#ifndef SPINLOCK_H
#define SPINLOCK_H

namespace lib {

struct spinlock {
	spinlock() {
		__atomic_clear(&_locked, __ATOMIC_RELEASE);
	}

	~spinlock() = default;

	spinlock(const spinlock &) = delete;
	spinlock &operator=(const spinlock &) = delete;

	void lock() {
		while (__atomic_test_and_set(&_locked, __ATOMIC_ACQUIRE))
			;
	}

	bool try_lock() {
		return !__atomic_test_and_set(&_locked, __ATOMIC_ACQUIRE);
	}

	void unlock() {
		__atomic_clear(&_locked, __ATOMIC_RELEASE);
	}

private:
	bool _locked;
};

} // namespace lib

#endif

#pragma once

#include <mem/mem.hpp>
#include <stdint.h>
#include <utility>

namespace mem {
	struct buffer {
		friend void swap(buffer &a, buffer &b) {
			using std::swap;
			swap(a.buffer_, b.buffer_);
			swap(a.size_, b.size_);
		}

		buffer()
		:buffer_{nullptr}, size_{0} {}

		buffer(void *ptr, size_t size)
		:buffer_{ptr}, size_{size} {}

		buffer(size_t size)
		:buffer_{get_allocator().allocate(size)}, size_{size} {}

		~buffer() {
			get_allocator().deallocate(buffer_, size_);
		}

		buffer(const buffer &other) = delete;
		buffer &operator=(const buffer &other) = delete;

		buffer(buffer &&other) :buffer_{nullptr}, size_{0} {
			swap(*this, other);
		}

		buffer &operator=(buffer &&other) {
			swap(*this, other);
			return *this;
		}

		void *data() {
			return buffer_;
		}

		template <typename T>
		T *data_as() {
			return reinterpret_cast<T *>(buffer_);
		}

		size_t size() const {
			return size_;
		}
	private:
		void *buffer_;
		size_t size_;
	};
} // namespace mem

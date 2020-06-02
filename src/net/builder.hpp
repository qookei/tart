#pragma once

#include <mem/buffer.hpp>
#include <frg/tuple.hpp>
#include <type_traits>
#include <concepts>

namespace net {

	struct plain_buffer {
		size_t size;
		void *data;

		size_t get_size() {
			return size;
		}

		void *to_bytes(void *ptr) {
			memcpy(ptr, data, size);
			return static_cast<uint8_t *>(ptr) + size;
		}
	};

	template <typename T>
	concept requires_checksum = requires (T a, void *b, ptrdiff_t c, size_t d) {
		{ a.do_checksum(b, c, d) } -> std::same_as<void>;
	};

	template <typename ...Args>
	mem::buffer build_packet(Args &&...args) {
		size_t total_size;
		auto parts = frg::make_tuple(std::forward<Args>(args)...);

		// collect size
		total_size = [&parts]<size_t ...I>(std::index_sequence<I...>) {
			return (parts.template get<I>().get_size() + ...);
		}(std::make_index_sequence<sizeof...(Args)>{});

		mem::buffer buf{total_size};

		// generate data
		void *dest = buf.data();
		[&parts, &dest]<size_t ...I>(std::index_sequence<I...>) {
			((dest = parts.template get<I>().to_bytes(dest)), ...);
		}(std::make_index_sequence<sizeof...(Args)>{});

		// generate do checksums
		dest = buf.data();
		[&]<size_t ...I>(std::index_sequence<I...>) {
			auto do_csum = [&]<typename T>(T item) {
				if constexpr (requires_checksum<T>) {
					ptrdiff_t off = reinterpret_cast<uintptr_t>(dest) - reinterpret_cast<uintptr_t>(buf.data());
					item.do_checksum(buf.data(), off, total_size);
				}

				dest = static_cast<uint8_t *>(dest) + item.get_size();
			};

			(do_csum(parts.template get<I>()), ...);
		}(std::make_index_sequence<sizeof...(Args)>{});

		return buf;
	}

} // namespace net

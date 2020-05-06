#pragma once

#include <frg/array.hpp>
#include <stdint.h>
#include <lib/logger.hpp>

namespace transmit {

enum class mode {
	in,
	out_single,
	out_buffer,
	both
};

struct operation {
	uint8_t *in_ptr;
	uint8_t *out_ptr;
	uint8_t raw;
	size_t size;
	mode m;
};

template <typename ...Args>
auto send_bytes(Args &&...args) {
	return frg::array<operation, sizeof...(Args)>{operation{
		nullptr, nullptr, static_cast<uint8_t>(args), 1, mode::out_single}...};
}

template <typename T>
auto send_buffer(T *buffer, size_t size) {
	return frg::array<operation, 1>{operation{nullptr, buffer, 0, size, mode::out_buffer}};
}

template <size_t N, typename T>
auto send_buffer(T (&buffer)[N]) {
	return send_buffer(buffer, N);
}

template <typename ...Args>
auto recv_bytes(Args &...args) {
	return frg::array<operation, sizeof...(Args)>{operation{&args, nullptr, 0, 1, mode::in}...};
}

template <typename T>
auto recv_buffer(T *buffer, size_t size) {
	return frg::array<operation, 1>{operation{buffer, nullptr, 0, size, mode::in}};
}

template <size_t N, typename T>
auto recv_buffer(T (&buffer)[N]) {
	return recv_buffer(buffer, N);
}

template <typename T>
auto xfer_buffers(T *out, T *in, size_t size) {
	return frg::array<operation, 1>{operation{out, in, 0, size, mode::both}};
}

template <size_t N, typename T>
auto xfer_buffers(T (&out)[N], T (&in)[N]) {
	return xfer_buffers(out, in, N);
}

template <typename Transmitter, typename ...Args>
void do_transmission(Transmitter *transmitter, Args &&...args) {
	auto items = frg::array_concat<operation>(std::forward<Args>(args)...);

	size_t out_item = 0, in_item = 0;
	size_t out_off = 0, in_off = 0;
	size_t in_seq = 0, out_seq = 0, now_seq = 0;
	constexpr size_t seq_stuck = 100;
	bool in_stuck = false, out_stuck = false;

	transmitter->select();

	while (out_item < items.size() || in_item < items.size()) {
		size_t tmp = 0;
		while (out_item < items.size()
				&& transmitter->is_transmit_empty()) {
			auto &item = items[out_item];
			uint8_t v;

			switch(item.m) {
				case mode::in: v = 0; break;
				case mode::out_single: v = item.raw; break;
				case mode::out_buffer:
				case mode::both: v = item.out_ptr[out_off];
			}

			transmitter->send(v);
			tmp++;

			if (++out_off == item.size) {
				out_off = 0;
				out_item++;
			}
		}

		if (tmp) {
			out_stuck = false;
			out_seq = now_seq;
		}

		tmp = 0;
		while (in_item < items.size()
				&& (out_item == in_item ? out_off > in_off : out_item > in_item)
				&& transmitter->is_receive_not_empty()) {
			auto &item = items[in_item];
			uint8_t v = transmitter->recv();
			tmp++;

			switch(item.m) {
				case mode::out_single:
				case mode::out_buffer: /* discard v */ break;
				case mode::both:
				case mode::in: item.in_ptr[in_off] = v;
			}

			if (++in_off == item.size) {
				in_off = 0;
				in_item++;
			}
		}

		if (tmp) {
			in_stuck = false;
			in_seq = now_seq;
		}

		now_seq++;

		if (out_item > in_item && in_seq + seq_stuck < now_seq && !in_stuck) {
			lib::log("do_transmission: receiver stuck? in_seq = %lu, now_seq = %lu, in_item = %lu, out_item = %lu, items.size() = %lu\r\n", in_seq, now_seq, in_item, out_item, items.size());
			in_stuck = true;
		}

		// in_item is never larger than out_item
		if (out_item == in_item && out_seq + seq_stuck < now_seq && !out_stuck) {
			lib::log("do_transmission: transmitter stuck? out_seq = %lu, now_seq = %lu, in_item = %lu, out_item = %lu, items.size() = %lu\r\n", out_seq, now_seq, in_item, out_item, items.size());
			out_stuck = true;
		}
	}

	transmitter->deselect();
}

template <typename Transmitter, typename ...Args>
void do_transmission(Transmitter &transmitter, Args &&...args) {
	do_transmission(&transmitter, std::forward<Args>(args)...);
}

} // namespace transmit

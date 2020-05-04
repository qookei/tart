#pragma once

#include <stddef.h>

#include <async/basic.hpp>
#include <async/queue.hpp>
#include <mem/buffer.hpp>

namespace net {
	struct processor {
		processor()
		: raw_packet_queue_{mem::get_allocator()} {}

		async::detached process_packets();
		void push_packet(mem::buffer &&b);
	private:
		async::queue<mem::buffer, mem::allocator> raw_packet_queue_;
	};
} // namespace net

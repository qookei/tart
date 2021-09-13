#pragma once

#include <async/result.hpp>
#include <tart/net/dispatch.hpp>
#include <lib/logger.hpp>
#include <tart/net/ipv4.hpp>

namespace net {
	struct icmp_frame {
		static constexpr uint8_t echo_request_type = 8;
		static constexpr uint8_t echo_reply_type = 0;

		uint8_t type;
		uint8_t code;
		uint16_t csum;

		// echo
		uint16_t ident;
		uint16_t seq;

		void *payload;
		size_t payload_size;

		static icmp_frame from_ipv4_frame(ipv4_frame &data) {
			uint8_t *bytes = static_cast<uint8_t *>(data.payload);
			icmp_frame f;
			f.type = bytes[0];
			f.code = bytes[1];
			f.csum = (uint16_t(bytes[0]) << 8) | bytes[1];

			switch(f.type) {
				case echo_reply_type:
				case echo_request_type:
					f.ident = (uint16_t(bytes[4]) << 8) | bytes[5];
					f.seq = (uint16_t(bytes[6]) << 8) | bytes[7];
					f.payload = bytes + 8;
					f.payload_size = data.payload_size - 8;
					break;
			}

			return f;
		}

		size_t get_size() {
			switch (type) {
				case echo_reply_type:
				case echo_request_type:
					return 8 + payload_size;
			}

			return 4;
		}

		void *to_bytes(void *ptr) {
			uint8_t *buf = static_cast<uint8_t *>(ptr);
			*buf++ = type;
			*buf++ = code;

			// Checksum
			*buf++ = 0;
			*buf++ = 0;

			switch(type) {
				case echo_reply_type:
				case echo_request_type:
					*buf++ = ident >> 8;
					*buf++ = ident & 0xFF;

					*buf++ = seq >> 8;
					*buf++ = seq & 0xFF;

					memcpy(buf, payload, payload_size);
					buf += payload_size;
					break;
			}

			return buf;
		}

		void do_checksum (void *buf, ptrdiff_t offset, size_t) {
			auto hdr = static_cast<uint8_t *>(buf) + offset;

			uint16_t sum = checksum::compute(hdr, get_size());

			// Checksum
			hdr[2] = sum >> 8;
			hdr[3] = sum & 0xFF;
		}
	};

	struct icmp_processor {
		using from_frame_type = ipv4_frame;

		icmp_processor()
		: sender_{nullptr} { }

		void attach_sender(sender *s) { sender_ = s; }

		async::result<void> push_packet(mem::buffer &&b, ipv4_frame &&);
		bool matches(const ipv4_frame &f);

	private:
		sender *sender_;
	};

	static_assert(net::processor<icmp_processor>);
} // namespace net

#pragma once

#include <stdint.h>

#include <tart/net/dispatch.hpp>
#include <tart/net/checksum.hpp>
#include <tart/net/address.hpp>

namespace net {
	struct ipv4_frame {
		ipv4_addr source;
		ipv4_addr dest;
		uint8_t protocol;
		uint8_t ttl;
		uint16_t length;
		uint8_t ihl;

		void *payload;
		size_t payload_size;

		ethernet_frame ether;

		static ipv4_frame from_ethernet_frame(ethernet_frame &data) {
			uint8_t *bytes = static_cast<uint8_t *>(data.payload);
			ipv4_frame f;
			f.ether = data;
			f.source = ipv4_addr::from_bytes(bytes + 12);
			f.dest = ipv4_addr::from_bytes(bytes + 16);
			f.ihl = bytes[0] & 0xF;
			f.ttl = bytes[8];
			f.protocol = bytes[9];
			f.length = (uint16_t(bytes[2]) << 8) | bytes[3];
			f.payload = bytes + f.ihl * 4;
			f.payload_size = f.length - f.ihl * 4;

			return f;
		}

		size_t get_size() {
			return ihl * 4;
		}

		void *to_bytes(void *ptr) {
			uint8_t *buf = static_cast<uint8_t *>(ptr);

			// TODO: support options
			assert(ihl == 5);
			*buf++ = (4 << 4) | (ihl);
			*buf++ = 0;

			uint16_t size = payload_size + ihl * 4;
			*buf++ = size >> 8;
			*buf++ = size & 0xFF;

			// Identification
			*buf++ = 0;
			*buf++ = 0;

			// Flags & Fragment offset
			*buf++ = 0;
			*buf++ = 0;

			*buf++ = ttl;
			*buf++ = protocol;

			// Checksum
			*buf++ = 0;
			*buf++ = 0;

			buf = static_cast<uint8_t *>(source.to_bytes(buf));
			buf = static_cast<uint8_t *>(dest.to_bytes(buf));

			return buf;
		}

		void do_checksum (void *buf, ptrdiff_t offset, size_t l) {
			auto hdr = static_cast<uint8_t *>(buf) + offset;

			// HACK: update total length now that we know it
			uint16_t len = l - offset;
			hdr[2] = len >> 8;
			hdr[3] = len & 0xFF;

			uint16_t sum = checksum::compute(hdr, get_size());

			// Checksum
			hdr[10] = sum >> 8;
			hdr[11] = sum & 0xFF;
		}

		static constexpr uint8_t icmp_proto = 1;
		static constexpr uint8_t igmp_proto = 2;
		static constexpr uint8_t tcp_proto = 6;
		static constexpr uint8_t udp_proto = 17;
		static constexpr uint8_t encap_proto = 41;
		static constexpr uint8_t ospf_proto = 89;
		static constexpr uint8_t sctp_proto = 132;
	};

	template <processor ...Ts>
		requires (same_as<typename Ts::from_frame_type, ipv4_frame> && ...)
	struct ipv4_processor {
		using from_frame_type = ethernet_frame;

		void attach_sender(sender *) requires (sizeof...(Ts) == 0) { }
		void attach_sender(sender *s) requires (sizeof...(Ts) > 0) {
			[&]<size_t ...I>(std::index_sequence<I...>) {
				(processors_.template get<I>().attach_sender(s), ...);
			}(std::make_index_sequence<sizeof...(Ts)>{});
		}

		// TODO: reassemble fragmented packets
		async::result<void> push_packet(mem::buffer &&b, ethernet_frame &&f) {
			auto ipv4 = ipv4_frame::from_ethernet_frame(f);
			co_await dispatch_frame(std::move(b), std::move(ipv4), processors_);
		}

		bool matches(const ethernet_frame &f) {
			return f.type == ethernet_frame::ipv4_type;
		}

		template <size_t I>
		auto &nth_processor() {
			return processors_.template get<I>();
		}

	private:
		frg::tuple<Ts...> processors_;
	};
} // namespace net

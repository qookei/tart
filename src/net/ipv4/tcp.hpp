#pragma once

#include <async/recurring-event.hpp>
#include <async/result.hpp>
#include <net/dispatch.hpp>
#include <async/mutex.hpp>
#include <lib/logger.hpp>
#include <net/ipv4.hpp>
#include <net/arp.hpp>

#include <frg/list.hpp>

namespace net {
	struct tcp_frame {
		uint16_t src_port;
		uint16_t dest_port;
		uint16_t csum;

		uint32_t seq_num;
		uint32_t ack_num;

		bool fin;
		bool syn;
		bool rst;
		bool psh;
		bool ack;

		uint8_t data_offset;
		uint16_t window_size;

		const void *payload;
		size_t payload_size;

		ipv4_frame f;

		static tcp_frame from_ipv4_frame(ipv4_frame &data) {
			uint8_t *bytes = static_cast<uint8_t *>(data.payload);
			tcp_frame f;
			f.src_port = (uint16_t(bytes[0]) << 8) | bytes[1];
			f.dest_port = (uint16_t(bytes[2]) << 8) | bytes[3];
			f.seq_num = (uint32_t(bytes[4]) << 24) | (uint32_t(bytes[5]) << 16)
				| (uint32_t(bytes[6]) << 8) | bytes[7];
			f.ack_num = (uint32_t(bytes[8]) << 24) | (uint32_t(bytes[9]) << 16)
				| (uint32_t(bytes[10]) << 8) | bytes[11];
			f.data_offset = bytes[12] >> 4;
			f.fin = bytes[13] & (1 << 0);
			f.syn = bytes[13] & (1 << 1);
			f.rst = bytes[13] & (1 << 2);
			f.psh = bytes[13] & (1 << 3);
			f.ack = bytes[13] & (1 << 4);

			f.window_size = (uint16_t(bytes[14]) << 8) | bytes[15];

			f.csum = (uint16_t(bytes[16]) << 8) | bytes[17];
			f.f = data;

			f.payload = bytes + f.data_offset * 4;
			f.payload_size = data.payload_size - f.data_offset * 4;

			return f;
		}

		size_t get_size() {
			return payload_size + data_offset * 4;
		}

		void *to_bytes(void *ptr) {
			uint8_t *buf = static_cast<uint8_t *>(ptr);
			*buf++ = src_port >> 8;
			*buf++ = src_port & 0xFF;

			*buf++ = dest_port >> 8;
			*buf++ = dest_port & 0xFF;

			*buf++ = seq_num >> 24;
			*buf++ = (seq_num >> 16) & 0xFF;
			*buf++ = (seq_num >> 8) & 0xFF;
			*buf++ = seq_num & 0xFF;

			*buf++ = ack_num >> 24;
			*buf++ = (ack_num >> 16) & 0xFF;
			*buf++ = (ack_num >> 8) & 0xFF;
			*buf++ = ack_num & 0xFF;

			*buf++ = data_offset << 4;

			*buf++ = (fin ? (1 << 0) : 0)
				| (syn ? (1 << 1) : 0)
				| (rst ? (1 << 2) : 0)
				| (psh ? (1 << 3) : 0)
				| (ack ? (1 << 4) : 0);

			*buf++ = (window_size) >> 8;
			*buf++ = (window_size) & 0xFF;

			// Checksum
			*buf++ = 0;
			*buf++ = 0;

			// Urgent pointer
			*buf++ = 0;
			*buf++ = 0;

			memcpy(buf, payload, payload_size);
			buf += payload_size;

			return buf;
		}

		void do_checksum(void *buf, ptrdiff_t offset, size_t) {
			auto hdr = static_cast<uint8_t *>(buf) + offset;

			uint8_t phdr[12];
			f.source.to_bytes(phdr);
			f.dest.to_bytes(phdr + 4);
			phdr[8] = 0;
			phdr[9] = ipv4_frame::tcp_proto;
			phdr[10] = get_size() >> 8;
			phdr[11] = get_size() & 0xFF;

			checksum csum;
			csum.update(phdr, 12);
			csum.update(hdr, get_size());

			uint16_t sum = csum.value();

			// Checksum
			hdr[16] = sum >> 8;
			hdr[17] = sum & 0xFF;
		}
	};

	struct tcp_processor {
		using from_frame_type = ipv4_frame;

		tcp_processor()
		: sender_{nullptr}, sockets_{} { }

		void attach_sender(sender *s) { sender_ = s; }

		async::result<void> push_packet(mem::buffer &&b, ipv4_frame &&);
		bool matches(const ipv4_frame &f);

		struct tcp_socket {
			friend tcp_processor;

			async::result<frg::optional<mem::buffer>> recv();
			async::result<bool> send(const void *buf, size_t size);

			async::result<void> close();

			ipv4_addr ip() const {
				return ip_;
			}

			uint16_t out_port() const {
				return out_port_;
			}

			uint16_t in_port() const {
				return in_port_;
			}

			bool connected() {
				return state_ == socket_state::want_ack
					|| state_ == socket_state::connected;
			}
		private:
			enum class socket_state {
				none,
				want_syn,
				want_syn_ack,
				want_ack,
				connected,
				want_fin_ack,
				closed
			};

			tcp_socket(tcp_processor *parent, uint16_t in_port,
					uint16_t out_port, ipv4_addr ip, socket_state state)
			: parent_{parent}, in_port_{in_port}, out_port_{out_port},
				ip_{ip}, peer_mac_{}, state_{state}, next_state_{},
				in_seq_{}, out_seq_{0}, next_out_seq_{}, remote_window_{},
				local_window_{0xFFFF}, notify_{}, closed_{},
				recv_queue_{mem::get_allocator()}, send_mutex_{},
				cancel_closed_{}, cancel_closed_token_{cancel_closed_}, node_{} { }

			tcp_processor *parent_;

			uint16_t in_port_;
			uint16_t out_port_;
			ipv4_addr ip_;
			mac_addr peer_mac_;

			socket_state state_, next_state_;

			uint32_t in_seq_;
			uint32_t out_seq_;

			uint32_t next_out_seq_;

			uint16_t remote_window_;
			uint16_t local_window_;

			async::result<void> process_packet(tcp_frame tcp, mem::buffer &&b);
			async::result<void> send_syn_ack();
			async::result<void> send_ack();
			async::result<void> send_fin_ack();
			async::result<void> send_syn();

			async::recurring_event notify_;
			async::recurring_event closed_;

			async::queue<mem::buffer, mem::allocator> recv_queue_;
			async::mutex send_mutex_;
			async::cancellation_event cancel_closed_;
			async::cancellation_token cancel_closed_token_;

			frg::default_list_hook<tcp_socket> node_;
		};

		async::result<tcp_socket *> listen(uint16_t port);
		tcp_socket *connect(ipv4_addr ip, uint16_t port);

		void set_arp_processor(arp_processor *ap) {
			arp_processor_ = ap;
		}

	private:
		sender *sender_;

		frg::intrusive_list<
			tcp_socket,
			frg::locate_member<
				tcp_socket,
				frg::default_list_hook<tcp_socket>,
				&tcp_socket::node_
			>
		> sockets_;

		arp_processor *arp_processor_;
	};

	static_assert(net::processor<tcp_processor>);
} // namespace net

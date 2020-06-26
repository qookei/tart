#include "tcp.hpp"
#include <net/builder.hpp>
#include <lib/logger.hpp>

//#define DEBUG_TCP

namespace net {

async::result<void> tcp_processor::push_packet(mem::buffer &&b, ipv4_frame &&f) {
	tcp_frame tcp = tcp_frame::from_ipv4_frame(f);
#ifdef DEBUG_TCP
	lib::log("tcp_processor::push_packet: from %u to %u\r\n",
		tcp.src_port, tcp.dest_port);

	lib::log("tcp_processor::push_packet: seq %u, ack %u\r\n",
		tcp.seq_num, tcp.ack_num);

	lib::log("tcp_processor::push_packet: window size %u\r\n",
		tcp.window_size);

	lib::log("tcp_processor::push_packet: %s%s%s%s%s\r\n",
		tcp.fin ? "FIN " : "",
		tcp.syn ? "SYN " : "",
		tcp.rst ? "RST " : "",
		tcp.psh ? "PSH " : "",
		tcp.ack ? "ACK " : "");
#endif
	for (auto s : sockets_) {
		if (tcp.dest_port != s->in_port_)
			continue;

		// this socket has an on-going connection already
		if (s->state_ != tcp_socket::socket_state::want_syn
				&& s->state_ != tcp_socket::socket_state::want_syn_ack
				&& s->out_port_ != tcp.src_port)
			continue;

		co_await s->process_packet(tcp, std::move(b));
		break;
	}
}

bool tcp_processor::matches(const ipv4_frame &f) {
	return f.protocol == ipv4_frame::tcp_proto;
}

auto tcp_processor::listen(uint16_t port) -> async::result<tcp_socket *> {
	tcp_socket *s = new tcp_socket{this, port, 0, {}, tcp_socket::socket_state::want_syn};
	sockets_.push_back(s);
	co_await s->notify_.async_wait();

	co_return s;
}

auto tcp_processor::connect(ipv4_addr ip, uint16_t port) -> tcp_socket * {
	// TODO: implement me
	/*
	uint16_t in_port = sender_->port_allocator().allocate();
	tcp_socket *s = new tcp_socket{this, in_port, port, ip, tcp_socket::socket_state::want_syn_ack};
	sockets_.push_back(s);

	return s;*/
	return nullptr;
}

async::result<void> tcp_processor::tcp_socket::process_packet(tcp_frame tcp, mem::buffer &&b) {
	if (state_ == socket_state::want_ack && tcp.ack && !tcp.syn
			&& in_seq_ == tcp.seq_num) {
		out_seq_ = next_out_seq_;
		state_ = next_state_;
		next_state_ = socket_state::none;

		if (state_ == socket_state::connected)
			notify_.ring();

		if (state_ == socket_state::closed)
			closed_.ring();

		co_return;
	}

	if (state_ == socket_state::want_syn && tcp.syn && !tcp.ack) {
		in_seq_ = tcp.seq_num + 1;
		ip_ = tcp.f.source;
		out_port_ = tcp.src_port;
		if (!peer_mac_) peer_mac_ = tcp.f.ether.source;

		next_out_seq_ = out_seq_ + 1;
		next_state_ = socket_state::connected;
		state_ = socket_state::want_ack;
		co_await send_syn_ack();
	}

	if (state_ == socket_state::want_syn_ack && tcp.syn && tcp.ack) {
		in_seq_ = tcp.seq_num + 1;
		ip_ = tcp.f.source;
		if (!peer_mac_) peer_mac_ = tcp.f.ether.source;

		state_ = socket_state::connected;
		co_await send_ack();
	}

	if (state_ == socket_state::connected) {
		assert(tcp.ack);
		if (tcp.fin) {
			in_seq_ = tcp.seq_num + 1;
			next_state_ = socket_state::closed;
			state_ = socket_state::want_ack;
			co_await send_fin_ack();
		} else if (in_seq_ >= tcp.seq_num) {
			in_seq_ = tcp.seq_num + tcp.payload_size;
			co_await send_ack();

			if (tcp.payload_size) {
				mem::buffer b{tcp.payload_size};
				memcpy(b.data(), tcp.payload, tcp.payload_size);
				recv_queue_.emplace(std::move(b));
			}
		}
	}

	if (state_ == socket_state::want_fin_ack && tcp.fin && tcp.ack) {
		in_seq_ = tcp.seq_num + 1;
		out_seq_++;
		co_await send_ack();
		state_ = socket_state::closed;
		closed_.ring();
	}
}

async::result<void> tcp_processor::tcp_socket::send_syn_ack() {
	auto packet = build_packet(
		ethernet_frame{parent_->sender_->mac(), peer_mac_, ethernet_frame::ipv4_type, nullptr, 0},
		ipv4_frame{parent_->sender_->ip(), ip_, ipv4_frame::tcp_proto, 64, 0, 5, nullptr, 0, {}},
		tcp_frame{in_port_, out_port_, 0, out_seq_, in_seq_, false, true, false, false, true, 5, local_window_,
			nullptr, 0, {parent_->sender_->ip(), ip_}}
	);

	co_await parent_->sender_->send_packet(std::move(packet));
}

async::result<void> tcp_processor::tcp_socket::send_ack() {
	auto packet = build_packet(
		ethernet_frame{parent_->sender_->mac(), peer_mac_, ethernet_frame::ipv4_type, nullptr, 0},
		ipv4_frame{parent_->sender_->ip(), ip_, ipv4_frame::tcp_proto, 64, 0, 5, nullptr, 0, {}},
		tcp_frame{in_port_, out_port_, 0, out_seq_, in_seq_, false, false, false, false, true, 5, local_window_,
			nullptr, 0, {parent_->sender_->ip(), ip_}}
	);

	co_await parent_->sender_->send_packet(std::move(packet));
}

async::result<void> tcp_processor::tcp_socket::send_fin_ack() {
	auto packet = build_packet(
		ethernet_frame{parent_->sender_->mac(), peer_mac_, ethernet_frame::ipv4_type, nullptr, 0},
		ipv4_frame{parent_->sender_->ip(), ip_, ipv4_frame::tcp_proto, 64, 0, 5, nullptr, 0, {}},
		tcp_frame{in_port_, out_port_, 0, out_seq_, in_seq_, true, false, false, false, true, 5, local_window_,
			nullptr, 0, {parent_->sender_->ip(), ip_}}
	);

	co_await parent_->sender_->send_packet(std::move(packet));
}

async::result<mem::buffer> tcp_processor::tcp_socket::recv() {
	co_return std::move(*co_await recv_queue_.async_get());
}

async::result<void> tcp_processor::tcp_socket::send(void *buf, size_t size) {
	assert(state_ == socket_state::connected);
	co_await send_mutex_.async_lock();

	auto packet = build_packet(
		ethernet_frame{parent_->sender_->mac(), peer_mac_, ethernet_frame::ipv4_type, nullptr, 0},
		ipv4_frame{parent_->sender_->ip(), ip_, ipv4_frame::tcp_proto, 64, 0, 5, nullptr, 0, {}},
		tcp_frame{in_port_, out_port_, 0, out_seq_, in_seq_, false, false, false, true, true, 5, local_window_,
			buf, size, {parent_->sender_->ip(), ip_}}
	);

	out_seq_ += size;

	next_out_seq_ = out_seq_;
	next_state_ = socket_state::connected;
	state_ = socket_state::want_ack;

	co_await parent_->sender_->send_packet(std::move(packet));

	send_mutex_.unlock();
}

async::result<void> tcp_processor::tcp_socket::close() {
	if (state_ == socket_state::connected || (state_ == socket_state::want_ack
				&& next_state_ == socket_state::connected)) {
		// TODO: send fin ack, wait for fin ack, send ack...
		//in_seq_++;
		state_ = socket_state::want_fin_ack;
		co_await send_fin_ack();
		if (state_ != socket_state::closed)
			co_await closed_.async_wait();
	} else if (state_ == socket_state::want_ack
			&& next_state_ == socket_state::closed) {
		// waiting for final ack
		co_await closed_.async_wait();
	} else if (state_ != socket_state::closed) {
		// some other state
		// wait for connection to be established
		co_await notify_.async_wait();
		co_await close();
		co_return;
	}

	parent_->sockets_.erase(this);
	if (port_allocator::base <= in_port_)
		parent_->sender_->port_allocator().free(in_port_);
	delete this;
}

} // namespace net

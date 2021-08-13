#include "udp.hpp"
#include <net/builder.hpp>

namespace net {

async::result<void> udp_processor::push_packet(mem::buffer &&b, ipv4_frame &&f) {
	udp_frame udp = udp_frame::from_ipv4_frame(f);

	for (auto s : sockets_) {
		if (udp.dest_port != s->in_port_)
			continue;

		if (s->listening_) {
			s->listening_ = false;
			s->out_port_ = udp.src_port;
			s->ip_ = f.source;
			s->notify_.raise();
		}

		if (f.source == s->ip_) {
			mem::buffer b{udp.length};
			memcpy(b.data(), udp.payload, udp.length);
			s->recv_queue_.emplace(std::move(b));
			break;
		}
	}

	co_return;
}

bool udp_processor::matches(const ipv4_frame &f) {
	return f.protocol == ipv4_frame::udp_proto;
}

auto udp_processor::listen(uint16_t port) -> async::result<udp_socket *> {
	uint16_t out_port = sender_->port_allocator().allocate();
	udp_socket *s = new udp_socket{this, port, out_port, {}, true};
	sockets_.push_back(s);
	co_await s->notify_.async_wait();

	co_return s;
}

auto udp_processor::connect(ipv4_addr ip, uint16_t port) -> udp_socket * {
	uint16_t in_port = sender_->port_allocator().allocate();
	udp_socket *s = new udp_socket{this, in_port, port, ip, false};
	sockets_.push_back(s);

	return s;
}

async::result<mem::buffer> udp_processor::udp_socket::recv() {
	co_return std::move(*co_await recv_queue_.async_get());
}

async::result<void> udp_processor::udp_socket::send(void *buf, size_t size) {
	auto target_mac = ip_ != ipv4::broadcast
			? (co_await parent_->arp_processor_->mac_of(ip_))
			: mac::broadcast;

	auto packet = build_packet(
		ethernet_frame{parent_->sender_->mac(), target_mac, ethernet_frame::ipv4_type, nullptr, 0},
		ipv4_frame{parent_->sender_->ip(), ip_, ipv4_frame::udp_proto, 64, 0, 5, nullptr, 0, {}},
		udp_frame{in_port_, out_port_, static_cast<uint16_t>(size), 0, buf, {}}
	);

	co_await parent_->sender_->send_packet(std::move(packet));
}

void udp_processor::udp_socket::close() {
	parent_->sockets_.erase(this);
	if (port_allocator::base <= in_port_)
		parent_->sender_->port_allocator().free(in_port_);
	delete this;
}

} // namespace net

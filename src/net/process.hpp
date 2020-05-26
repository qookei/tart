#pragma once

#include <stddef.h>

#include <async/basic.hpp>
#include <async/result.hpp>
#include <async/queue.hpp>
#include <mem/buffer.hpp>

#include <frg/tuple.hpp>

#include <net/mac.hpp>
#include <net/ether.hpp>

#include <concepts>

namespace net {
	template <typename Awaiter, typename T>
	concept awaits_to = requires (Awaiter &&a) {
		{ operator co_await(std::move(a)).await_resume() } -> std::same_as<T>;
	};

	template <typename T>
	concept processor = requires (T a, mem::buffer &&b, typename T::from_frame_type &&f, const typename T::from_frame_type &f2) {
		typename T::from_frame_type;
		{ a.push_packet(std::move(b), std::move(f)) } -> awaits_to<void>;
		{ a.matches(f2) } -> std::same_as<bool>;
	};

	struct null_processor {
		using from_frame_type = ethernet_frame;

		async::result<void> push_packet(mem::buffer &&, ethernet_frame &&) { co_return; }
		bool matches(const ethernet_frame &) { return false; }
	};

	static_assert(processor<null_processor>);

	template <typename T>
	concept nic = requires (T a, mem::buffer &&b) {
		{ a.recv_packet() } -> awaits_to<mem::buffer>;
		{ a.send_packet(std::move(b)) } -> awaits_to<void>;
		{ a.mac() } -> std::same_as<net::mac>;
		{ a.run() } -> std::same_as<async::detached>;
	};

	template <nic Nic, processor ...Ts>
	struct ether_dispatcher {
		ether_dispatcher(Nic &nic)
		: nic_{&nic}, processors_{} {}

		async::detached run() {
			nic_->run();

			while (true) {
				auto buffer = co_await nic_->recv_packet();
				auto ether = ethernet_frame::from_bytes(buffer.data(), buffer.size());

				co_await [&]<size_t ...I>(std::index_sequence<I...>) -> async::result<void> {
					auto f = [&](auto &proc) -> async::result<bool> {
						if (proc.matches(ether)) {
							co_await proc.push_packet(std::move(buffer), std::move(ether));
							co_return false;
						}

						co_return true;
					};

					((co_await f(processors_.template get<I>())) && ...);
				}(std::make_index_sequence<sizeof...(Ts)>{});
			}
		}
	private:
		Nic *nic_;
		frg::tuple<Ts...> processors_;
	};
} // namespace net

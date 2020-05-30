#pragma once

#include <stddef.h>

#include <async/basic.hpp>
#include <async/result.hpp>
#include <async/queue.hpp>
#include <mem/buffer.hpp>

#include <frg/tuple.hpp>
#include <type_traits>

#include <net/mac.hpp>
#include <net/ether.hpp>

#include <concepts>

namespace net {
	template <typename Awaiter, typename T>
	concept awaits_to = requires (Awaiter &&a) {
		{ operator co_await(std::move(a)).await_resume() } -> std::same_as<T>;
	};

	struct sender {
		virtual ~sender() { }
		virtual async::result<void> send_packet(mem::buffer &&) = 0;
		virtual net::mac mac() = 0;
	};

	template <typename T>
	concept processor = requires (T a, mem::buffer &&b,
			typename T::from_frame_type &&f,
			const typename T::from_frame_type &f2, sender *s) {
		typename T::from_frame_type;
		{ a.attach_sender(s) } -> std::same_as<void>;
		{ a.push_packet(std::move(b), std::move(f)) } -> awaits_to<void>;
		{ a.matches(f2) } -> std::same_as<bool>;
	};

	template <typename T>
	concept nic = requires (T a, mem::buffer &&b) {
		{ a.recv_packet() } -> awaits_to<mem::buffer>;
		{ a.send_packet(std::move(b)) } -> awaits_to<void>;
		{ a.mac() } -> std::same_as<net::mac>;
		{ a.run() } -> std::same_as<async::detached>;
	};

	template <typename From>
	async::result<void> dispatch_frame(mem::buffer &&, From &&, frg::tuple<> &) {
		// no op
		co_return;
	}

	template <typename From, processor ...Ts>
	async::result<void> dispatch_frame(mem::buffer &&bu, From &&fr, frg::tuple<Ts...> &pr) {
		co_await [&]<size_t ...I>(std::index_sequence<I...>) -> async::result<void> {
			auto f = [&]<typename T>(T &proc) -> async::result<bool> {
				static_assert(std::is_same_v<From, typename T::from_frame_type>);
				if (proc.matches(fr)) {
					co_await proc.push_packet(std::move(bu), std::move(fr));
					co_return false;
				}

				co_return true;
			};

			((co_await f(pr.template get<I>())) && ...);
		}(std::make_index_sequence<sizeof...(Ts)>{});
	}

	template <nic Nic, processor ...Ts>
		requires (std::same_as<typename Ts::from_frame_type, ethernet_frame> && ...)
	struct ether_dispatcher : public sender {
		ether_dispatcher(Nic &nic)
		: nic_{&nic}, processors_{} {
			[&]<size_t ...I>(std::index_sequence<I...>) {
				(processors_.template get<I>().attach_sender(this), ...);
			}(std::make_index_sequence<sizeof...(Ts)>{});
		}

		async::detached run() {
			nic_->run();

			while (true) {
				auto buffer = co_await nic_->recv_packet();
				auto ether = ethernet_frame::from_bytes(buffer.data(), buffer.size());
				co_await dispatch_frame(std::move(buffer), std::move(ether), processors_);
			}
		}

		async::result<void> send_packet(mem::buffer &&b) override {
			co_await nic_->send_packet(std::move(b));
		}

		net::mac mac() override {
			return nic_->mac();
		}

		template <size_t I>
		auto &nth_processor() {
			return processors_.template get<I>();
		}

	private:
		Nic *nic_;
		frg::tuple<Ts...> processors_;
	};
} // namespace net

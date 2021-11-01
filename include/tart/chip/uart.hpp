#pragma once

#include <stddef.h>

#include <arch/mem_space.hpp>

#include <tart/irq.hpp>

namespace tart::chip {
	struct uart {
		constexpr uart(int nth, uintptr_t base)
		: nth_{nth}, space_{base} { }

		void init(int baud);
		void send_sync(const void *data, size_t size);
		void send_sync(const char *data);

		size_t recv_sync(void *data, size_t max_size);

		bool tx_full() const;
		bool rx_empty() const;

		int nth() const {
			return nth_;
		}

		void handle_irq();

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
		template <typename Receiver>
		struct await_rx_operation : private irq_node {
			await_rx_operation(uart *self, Receiver r)
			: irq_node{}, self_{self}, r_{std::move(r)} { }

			bool start_inline() {
				if (!self_->rx_empty()) {
					async::execution::set_value_inline(r_);
					return true;
				}

				self_->pending_rx_.push_back(this);

				return false;
			}

		private:
			void complete() override {
				async::execution::set_value_noinline(r_);
			}

			uart *self_;
			Receiver r_;
		};
#pragma GCC diagnostic pop

		struct await_rx_sender {
			using value_type = void;

			template <typename Receiver>
			await_rx_operation<Receiver> connect(Receiver r) {
				return {self, std::move(r)};
			}

			async::sender_awaiter<await_rx_sender, void>
			operator co_await() {
				return {*this};
			}

			uart *self;
		};

		await_rx_sender await_rx() {
			return {this};
		}

	private:
		int nth_;
		::arch::mem_space space_;

		frg::intrusive_list<
			irq_node,
			frg::locate_member<
				irq_node,
				frg::default_list_hook<irq_node>,
				&irq_node::hook_
			>
		> pending_rx_;
	};

#if defined (TART_CHIP_RP2040)
	inline constinit uart uart0{0, 0x40034000};
	inline constinit uart uart1{1, 0x40038000};
#endif

} // namespace tart::chip

#pragma once

#include <tart/irq.hpp>
#include <async/basic.hpp>
#include <tart/arch/wait.hpp>

namespace tart {
	inline volatile uint64_t irq_seq_ = 0;

	struct irq_waiter {
		void wait() {
			while (last_seq_ == irq_seq_) {
				arch::wait_for_irq();
			}

			last_seq_ = irq_seq_;

			while (!pending_.empty()) {
				auto node = pending_.pop_front();
				node->complete();
			}
		}

		void post(irq_node *node) {
			pending_.push_back(node);
		}

	private:
		uint64_t last_seq_ = 0;

		frg::intrusive_list<
			irq_node,
			frg::locate_member<
				irq_node,
				frg::default_list_hook<irq_node>,
				&irq_node::hook_
			>
		> pending_;
	};

	struct irq_service {
		constexpr irq_service(irq_waiter *w)
		: w_{w} { }

		void wait() {
			w_->wait();
		}
	private:
		irq_waiter *w_;
	};

	inline constinit irq_waiter irq_w;
	inline constinit irq_service irq_s{&irq_w};
} // namespace tart

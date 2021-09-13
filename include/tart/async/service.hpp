#pragma once

#include <frg/list.hpp>
#include <tart/mem/mem.hpp>
#include <async/basic.hpp>

namespace service {
	struct pollable {
		virtual ~pollable() {}
		virtual bool do_poll() = 0;

		frg::default_list_hook<pollable> hook_;
	};

	struct io_service {
		io_service(async::run_queue *rq)
		: rq_{rq} {}

		void wait() {
			// TODO
			async::queue_scope qs{rq_};
			for (auto w : waiters_) {
				if (w->do_poll())
					break;
			}
		}

		void attach_waiter(pollable &obj) {
			waiters_.push_back(&obj);
		}

	private:
		async::run_queue *rq_;
		frg::intrusive_list<
			pollable,
			frg::locate_member<
				pollable,
				frg::default_list_hook<pollable>,
				&pollable::hook_
			>
		> waiters_;
	};
} // namespace service

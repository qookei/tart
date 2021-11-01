#pragma once

#include <tart/arch/wait.hpp>

namespace tart {
	inline volatile uint64_t irq_seq_ = 0;

	struct irq_service {
		void wait() {
			while (last_seq_ == irq_seq_) {
				arch::wait_for_irq();
			}

			last_seq_ = irq_seq_;
		}

	private:
		uint64_t last_seq_ = 0;
	};


} // namespace tart

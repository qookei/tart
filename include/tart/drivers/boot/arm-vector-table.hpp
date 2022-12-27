#pragma once

#include <frg/array.hpp>
#include <stddef.h>

namespace tart {
	namespace detail {
		extern "C" char core0_stack[];
		extern "C" void _start();

		template <void (*Fn)(void *)>
		[[gnu::naked]] void vector_fn() {
			asm volatile ("mov r0, sp\n\tb %c0"
					:: "i"(Fn) : "r0");
		}

		template <size_t N>
		void irq_handler_cb(void *ctx) {
			(void) ctx;
			// TODO: Implement
		}
	} // namespace detail

	template <size_t NIrqs>
	struct arm_vector_table {
		using fn_type = void (*)();

		void *stack;

		fn_type reset;

		fn_type nmi;		// All
		fn_type hard_fault;	// All
		fn_type mm_fault;	// v7-M, v8-M Mainline
		fn_type bus_fault;	// v7-M, v8-M Mainline
		fn_type usage_fault;	// v7-M, v8-M Mainline
		fn_type secure_fault;	// v8-M Mainline
		fn_type reserved1[3] {};
		fn_type sv_call;	// All
		fn_type debug_monitor;	// v7-M, v8-M Mainline
		fn_type reserved2 {};
		fn_type pend_sv_call;	// All
		fn_type systick;	// All

		frg::array<fn_type, NIrqs> irqs;

		constexpr static arm_vector_table generate() {
			return {
				.stack = detail::core0_stack,

				.reset = &detail::_start,

				.nmi = nullptr,
				.hard_fault = nullptr,
				.mm_fault = nullptr,
				.bus_fault = nullptr,
				.usage_fault = nullptr,
				.secure_fault = nullptr,
				.sv_call = nullptr,
				.debug_monitor = nullptr,
				.pend_sv_call = nullptr,
				.systick = nullptr,
				.irqs = make_irq_handlers_(),
			};
		}

	private:
		constexpr static frg::array<fn_type, NIrqs> make_irq_handlers_() {
			return [] <size_t ...I>(std::index_sequence<I...>) {
				return frg::array<fn_type, NIrqs>{
					&detail::vector_fn<detail::irq_handler_cb<I>>...};
			} (std::make_index_sequence<NIrqs>{});
		}
	};
} // namespace tart

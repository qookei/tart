#pragma once

#include <periph/gpio.hpp>

namespace spi {
	struct spi {
		spi(int nth) :nth_(nth) {}
		void setup(int clock_div, int mode = 0, bool msb = true);

		bool is_transmit_empty();
		bool is_receive_not_empty();

		void send(uint8_t value);
		uint8_t recv();

		void select();
		void deselect();

	private:
		int nth_;
	};

	// Allows for attaching a chip select pin to the device
	struct spi_dev {
		spi_dev(spi *iface, gpio::pin cs_pin, bool act_st = false)
		: iface_{iface}, cs_pin_{cs_pin}, act_st_{act_st} {
			gpio::setup(cs_pin_, gpio::mode::output_50mhz, gpio::config::push_pull);
			gpio::set(cs_pin_, !act_st_);
		}

		bool is_transmit_empty() {
			return iface_->is_transmit_empty();
		}

		bool is_receive_not_empty() {
			return iface_->is_receive_not_empty();
		}

		void send(uint8_t value) {
			iface_->send(value);
		}

		uint8_t recv() {
			return iface_->recv();
		}

		void select() {
			iface_->select();
			gpio::set(cs_pin_, act_st_);
		}

		void deselect() {
			gpio::set(cs_pin_, !act_st_);
			iface_->deselect();
		}
	private:
		spi *iface_;
		gpio::pin cs_pin_;
		bool act_st_;
	};
} // namespace spi

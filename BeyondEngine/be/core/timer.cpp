#include "timer.hpp"
#include <thread>

namespace be {
	namespace core {

		timer::timer() : m_targetFramerate(0), m_delta(0.0), m_secondsPerFrame(0.0f) {
			m_frameBeginTime = steady_clock::now();
		}

		timer::~timer() {}

		void timer::begin_frame() {
			time_point now = steady_clock::now();

			// get the delta to be used this frame
			m_delta = (now - m_frameBeginTime).count();
			m_frameBeginTime = now;
		}

		float timer::get() const {
			return static_cast<float>(m_delta);
		}

		double timer::get_double() const {
			return m_delta;
		}

		void timer::wait_for_frame() const {
			// skip if no framerate
			if (m_targetFramerate == 0) return;

			// wait in this loop for the frame to end
			while ((steady_clock::now() - m_frameBeginTime).count() < m_secondsPerFrame) {}
		}

		void timer::set_fps_target(uint32 targetFramerate) {
			m_targetFramerate = targetFramerate;
			if (m_targetFramerate != 0) m_secondsPerFrame = 1.0f / static_cast<float>(m_targetFramerate);
			else						m_secondsPerFrame = 0;
		}

		uint32 timer::get_fps_target() const {
			return m_targetFramerate;
		}

	}
}
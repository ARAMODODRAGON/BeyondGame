#ifndef BE_TIMER_HPP
#define BE_TIMER_HPP
#include <chrono>
#include "../common.hpp"

namespace be {

	class timer final {
	public:

		timer();

		~timer();

		// call at the begining of each frame
		void begin_frame();

		// returns the current delta
		float get() const;

		// returns the current delta as a double
		double get_double() const;

		// waits for the end of the frame, suspends excecution of this thread until the end of frame is reached
		void wait_for_frame() const;

		// sets the target fps
		// if set to 0 then framerate will be uncapped
		void set_fps_target(uint32 targetFramerate);

		// gets the target fps
		uint32 get_fps_target() const;

	private:
		using steady_clock = std::chrono::steady_clock;
		using duration = std::chrono::duration<double>;
		using time_point = std::chrono::time_point<steady_clock, duration>;

		time_point m_frameBeginTime;
		//time_point m_currentTime;

		uint32 m_targetFramerate;
		double m_secondsPerFrame;
		double m_delta;

	};

}

#endif // !BE_TIMER_HPP
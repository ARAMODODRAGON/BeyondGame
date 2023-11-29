#ifndef BE_INPUT_HPP
#define BE_INPUT_HPP
#include "../common.hpp"

namespace be {

	struct button {

		constexpr button()
			: m_held(false), m_changed(false) {}

		constexpr button(bool held, bool changed)
			: m_held(held), m_changed(changed) {}

		// returns the held state of the button
		operator bool() const { return m_held; }

		// returns the held state of the button
		bool held() const { return m_held; }

		// returns if this was pressed 
		bool pressed() const { return m_held && m_changed; }

		// returns if this was released
		bool released() const { return !m_held && m_changed; }

	private:
		bool m_held : 1;
		bool m_changed : 1;
	};

	enum class keycode : uint32_t {
		key_a,
		key_b,
		key_c,
		key_d,
		key_e,
		key_f,
		key_g,
		key_h,
		key_i,
		key_j,
		key_k,
		key_l,
		key_m,
		key_n,
		key_o,
		key_p,
		key_q,
		key_r,
		key_s,
		key_t,
		key_u,
		key_v,
		key_w,
		key_x,
		key_y,
		key_z,
		num_0,
		num_1,
		num_2,
		num_3,
		num_4,
		num_5,
		num_6,
		num_7,
		num_8,
		num_9,
		left_shift,
		right_shift,
		left_ctrl,
		right_ctrl,
		left_alt,
		right_alt,
		enter,
		escape,
		retur,
		backspace,
		tab,
		space,
		arrow_up,
		arrow_down,
		arrow_left,
		arrow_right,
		__SIZE__
	};

	class keyboard final {
	public:

		static button get(keycode code);

	private:
		struct key {
			bool held : 1;
			bool changed : 1;
		};
		static constexpr size_t c_keycount = static_cast<size_t>(keycode::__SIZE__);
		static inline key s_keys[c_keycount];
	public:
		static void __set(keycode code, bool state);
		static void __clear_changed_bits();
	};

	class mouse final {
	public:

		static button get_left();

		static button get_middle();
		
		static button get_right();
		
		static button get_fourth();
		
		static button get_fifth();
		
		static glm::vec2 get_position();
		
		static float get_scroll();

	private:
		static constexpr size_t c_button_count = 5;
		struct buttonvalue {
			bool held : 1;
			bool changed : 1;
			buttonvalue() : held(false), changed(false) {}
		};
		static inline glm::vec2 s_position = glm::vec2(0.0f);
		static inline float s_scrollDir = 0.0f;
		static inline buttonvalue s_buttons[c_button_count];
	public:
		static void __clear_changed_bits();
		static void __set_button(uint8_t buttonum, bool state);
		static void __set_position(const glm::vec2& position);
		static void __set_scroll(float scroll);
	};

}

#endif // !BE_INPUT_HPP
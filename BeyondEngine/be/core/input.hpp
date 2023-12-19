#ifndef BE_INPUT_HPP
#define BE_INPUT_HPP
#include "unordered_map"

namespace be {

	inline constexpr const char* __BUTTONS[] = {
		"key_a",
		"key_b",
		"key_c",
		"key_d",
		"key_e",
		"key_f",
		"key_g",
		"key_h",
		"key_i",
		"key_j",
		"key_k",
		"key_l",
		"key_m",
		"key_n",
		"key_o",
		"key_p",
		"key_q",
		"key_r",
		"key_s",
		"key_t",
		"key_u",
		"key_v",
		"key_w",
		"key_x",
		"key_y",
		"key_z",
		"num_0",
		"num_1",
		"num_2",
		"num_3",
		"num_4",
		"num_5",
		"num_6",
		"num_7",
		"num_8",
		"num_9",
		"left_shift",
		"right_shift",
		"left_ctrl",
		"right_ctrl",
		"left_alt",
		"right_alt",
		"key_enter",
		"key_escape",
		"key_return",
		"key_backspace",
		"key_tab",
		"key_space",
		"arrow_up",
		"arrow_down",
		"arrow_left",
		"arrow_right",
		"mouse_left",
		"mouse_middle",
		"mouse_right"
		"mouse_four",
		"mouse_five"
	};

	inline constexpr const char* __AXIS[] = {
		"mouse_x",
		"mouse_x",
		"mouse_y",
		"mouse_wheel"
	};

	static constexpr size_t __BUTTON_COUNT = sizeof(__BUTTONS) / sizeof(__BUTTONS[0]);
	static constexpr size_t __AXIS_COUNT = sizeof(__AXIS) / sizeof(__AXIS[0]);

	class input final {
	public:

		// returns the index of the button or axis name
		static size_t index(const char* cstr);

		// returns the state of the button
		static bool button(const char* cstr);

		// returns if button was pressed this frame
		static bool button_down(const char* cstr);

		// returns if button was released this frame
		static bool button_up(const char* cstr);


		// returns the state of the button using the index
		static bool button(size_t index);

		// returns if button was pressed this frame using the index
		static bool button_down(size_t index);

		// returns if button was released this frame using the index
		static bool button_up(size_t index);


		// returns the floating value of the axis
		static float axis(const char* cstr);

		// returns the floating value of the axis using the index
		static float axis(size_t index);

	private:
		struct buttondat final {
			bool held : 1;
			bool changed : 1;
			constexpr buttondat() : held(false), changed(false) {}
		};
		static inline buttondat _buttons[__BUTTON_COUNT];
		static inline float _axis[__AXIS_COUNT];
		static inline std::unordered_map<const char*, size_t> _indexes;
	public:
		static void __reset();
		static void __set_button(const char* cstr, bool state);
		static void __set_button(size_t index, bool state);
		static void __set_axis(const char* cstr, float a);
		static void __set_axis(size_t index, float a);
		input(const input&) = delete;
		input& operator=(const input&) = delete;
		input(input&&) = delete;
		input& operator=(input&&) = delete;
		input() = delete;
	};

	/*
	#define KEYCODES		\
	E(key_a),				\
	E(key_b),				\
	E(key_c),				\
	E(key_d),				\
	E(key_e),				\
	E(key_f),				\
	E(key_g),				\
	E(key_h),				\
	E(key_i),				\
	E(key_j),				\
	E(key_k),				\
	E(key_l),				\
	E(key_m),				\
	E(key_n),				\
	E(key_o),				\
	E(key_p),				\
	E(key_q),				\
	E(key_r),				\
	E(key_s),				\
	E(key_t),				\
	E(key_u),				\
	E(key_v),				\
	E(key_w),				\
	E(key_x),				\
	E(key_y),				\
	E(key_z),				\
	E(num_0),				\
	E(num_1),				\
	E(num_2),				\
	E(num_3),				\
	E(num_4),				\
	E(num_5),				\
	E(num_6),				\
	E(num_7),				\
	E(num_8),				\
	E(num_9),				\
	E(left_shift),			\
	E(right_shift),			\
	E(left_ctrl),			\
	E(right_ctrl),			\
	E(left_alt),			\
	E(right_alt),			\
	E(enter),				\
	E(escape),				\
	E(return_),				\
	E(backspace),			\
	E(tab),					\
	E(space),				\
	E(arrow_up),			\
	E(arrow_down),			\
	E(arrow_left),			\
	E(arrow_right)

	#define MOUSECODES		\
	E(left),				\
	E(middle),				\
	E(right),				\
	E(four),				\
	E(five)

	#define E(e) e
	enum class keycode : uint32_t {
		KEYCODES,
		__SIZE__
	};

	enum class mousecode : uint8 {
		MOUSECODES,
		__SIZE__
	};

	#undef E

	#define E(e) #e
	inline constexpr const char* __keycodestr[] = {KEYCODES};
	inline constexpr const char* __mousecodestr[] = {MOUSECODES};
	#undef E
	#undef KEYCODES

	class input final {
	public:

		// returns if key is held
		static bool get(keycode code);

		// returns if key is pressed this frame
		static bool down(keycode code);

		// returns if key is released this frame
		static bool up(keycode code);

		// returns the keycode as a string
		static const char* to_cstr(keycode code);

		// returns the string as the matching keycode
		static keycode keycode_from_cstr(const char* cstr);


		// returns if mouse button is held
		static bool get(mousecode code);

		// returns if mouse button is pressed this frame
		static bool down(mousecode code);

		// returns if mouse button is released this frame
		static bool up(mousecode code);

		// returns the cstr from the keycode
		static const char* to_cstr(mousecode code);

		static mousecode mousecode_from_cstr(const char* cstr);

		static glm::vec2 get_position();

		static float get_scroll();

	private:
		struct button {
			bool held : 1;
			bool changed : 1;
		};
		static constexpr size_t c_keycount = static_cast<size_t>(keycode::__SIZE__);
		static inline button s_keys[c_keycount];

		static constexpr size_t c_button_count = static_cast<size_t>(mousecode::__SIZE__);
		static inline glm::vec2 s_position = glm::vec2(0.0f);
		static inline float s_scrollDir = 0.0f;
		static inline button s_buttons[c_button_count];
	public:
		static void __set(keycode code, bool state);
		static void __set_button(mousecode code, bool state);
		static void __clear_changed_bits();
		static void __set_position(const glm::vec2& position);
		static void __set_scroll(float scroll);
	};

	*/
}

#endif // !BE_INPUT_HPP
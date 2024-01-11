#include "input.hpp"

//#define CODECASESTR(e, c) case e::c: return #c

namespace be {
	size_t input::index(const char* cstr) {
		auto it = _indexes.find(cstr);
		if (it != _indexes.end()) return it->second;
		return -1;
	}
	bool input::button(const char* cstr) {
		return button(index(cstr));
	}

	bool input::button_down(const char* cstr) {
		return button_down(index(cstr));
	}

	bool input::button_up(const char* cstr) {
		return button_up(index(cstr));
	}

	bool input::button(size_t index) {
		if (index == -1 || index >= __BUTTON_COUNT) return false;
		return _buttons[index].held;
	}

	bool input::button_down(size_t index) {
		if (index == -1 || index >= __BUTTON_COUNT) return false;
		return _buttons[index].held && _buttons[index].changed;
	}

	bool input::button_up(size_t index) {
		if (index == -1 || index >= __BUTTON_COUNT) return false;
		return !(_buttons[index].held) && _buttons[index].changed;
	}

	float input::axis(const char* cstr) {
		return axis(index(cstr) - __BUTTON_COUNT);
	}

	float input::axis(size_t index) {
		if (index == -1 || index >= __AXIS_COUNT) return false;
		return _axis[index];
	}


	void input::__reset() {
		if (_indexes.size() == 0) {
			for (size_t i = 0; i < __BUTTON_COUNT; i++) {
				_indexes.emplace(__BUTTONS[i], i);
			}
			for (size_t i = 0; i < __AXIS_COUNT; i++) {
				_indexes.emplace(__AXIS[i], i + __BUTTON_COUNT);
			}
		}

		for (size_t i = 0; i < __BUTTON_COUNT; i++) {
			_buttons[i].changed = false;
		}
		//for (size_t i = 0; i < __AXIS_COUNT; i++) {
		//	_axis[i] = 0.0f;
		//}
	}

	void input::__set_button(const char* cstr, bool state) {
		__set_button(index(cstr), state);
	}

	void input::__set_button(size_t index, bool state) {
		_buttons[index].changed = (_buttons[index].held != state);
		_buttons[index].held = state;
	}

	void input::__set_axis(const char* cstr, float a) {
		__set_axis(index(cstr) - __BUTTON_COUNT, a);
	}

	void input::__set_axis(size_t index, float a) {
		_axis[index] = a;
	}

	/*

	button keyboard::get(keycode code) {
		auto& key = s_keys[static_cast<uint32>(code)];
		return button(key.held, key.changed);
	}

	void keyboard::__clear_changed_bits() {
		for (size_t i = 0; i < c_keycount; i++) {
			s_keys[i].changed = false;
		}
	}

	void keyboard::__set(keycode code, bool state) {
		auto& key = s_keys[static_cast<uint32>(code)];
		key.changed = (key.held != state);
		key.held = state;
	}

	button mouse::get_left() {
		return button(s_buttons[0].held, s_buttons[0].changed);
	}

	button mouse::get_middle() {
		return button(s_buttons[1].held, s_buttons[1].changed);
	}

	button mouse::get_right() {
		return button(s_buttons[2].held, s_buttons[2].changed);
	}

	button mouse::get_fourth() {
		return button(s_buttons[3].held, s_buttons[3].changed);
	}

	button mouse::get_fifth() {
		return button(s_buttons[4].held, s_buttons[4].changed);
	}

	glm::vec2 mouse::get_position() {
		return s_position;
	}

	float mouse::get_scroll() {
		return s_scrollDir;
	}

	void mouse::__clear_changed_bits() {
		for (size_t i = 0; i < c_button_count; i++) {
			s_buttons[i].changed = false;
		}
		s_scrollDir = 0.0f;
	}

	void mouse::__set_button(uint8_t buttonum, bool state) {
		s_buttons[buttonum].changed = (s_buttons[buttonum].held != state);
		s_buttons[buttonum].held = state;
	}

	void mouse::__set_position(const glm::vec2& position) {
		s_position = position;
	}

	void mouse::__set_scroll(float scroll) {
		s_scrollDir = scroll;
	}

	*/
}
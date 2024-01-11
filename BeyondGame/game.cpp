#include <SDL.h>
#include "game.hpp"
#include "scenes\demo_scene.hpp"

#define WIN_SIZE glm::uvec2( 1280, 720 )
#define VIEW_HALFSIZE_X 128.0f * 0.5f
#define VIEW_HALFSIZE_Y 72.0f * 0.5f

int main(int argc, char** argv) {

	be::app::run<game>(WIN_SIZE, "DungeonRPG");

	return 0;
}


game::game() : m_scene(nullptr), m_view(1.0f), m_sceneIndex(-1) {
	m_time.set_fps_target(60);
}

game::~game() {}

void game::loadscene(size_t index) {
	scene* newscene = nullptr;
	m_sceneIndex = index;
	switch (index) {
		case 0: newscene = create<demo_scene>("Demo Scene"); break;
		default: 
			BE_WARNING("Scene index was not a registered index"); 
			m_sceneIndex = -1;
			break;
	}

	if (newscene != nullptr) {
		if (m_scene != nullptr) destroy(m_scene);
		m_scene = newscene;
	}
}

void game::ready() {
	m_canvas.reset(new be::canvas());
	loadscene(0);

	m_view = glm::ortho(1280.0f * -0.5f, 1280.0f * 0.5f, 720.0f * -0.5f, 720.0f * 0.5f);
}

void game::step() {
	m_time.begin_frame();
	window()->clear_screen(BE_COLOR_BLACK);

	game_step(m_time.get());
	on_step.emit(m_time.get());

	m_canvas->clear();
	if (m_scene) m_scene->draw(m_canvas.get());
	on_draw.emit(m_view);
	m_canvas->draw(m_view);

	// end frame
	window()->swap_buffers();
	m_time.wait_for_frame();
	tree()->handle_deletion();
}

void game::game_step(float delta) {
	if (m_scene) m_scene->step(delta);

	if (be::input::button_down("key_space")) {
		loadscene(m_sceneIndex);
	}

	if (!m_scene) {
		if (be::input::button_down("num_1")) {
			loadscene(0);
		}
	}
}

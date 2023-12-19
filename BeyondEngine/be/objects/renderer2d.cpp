#include "renderer2d.hpp"
#include "camera2d.hpp"
#include "spriterenderer.hpp"

namespace be {
	namespace graphics2d {

		renderer2d::renderer2d() : m_mainCamera(nullptr), m_batch(nullptr) {
			m_batch = new sprite_batch();
		}

		renderer2d::~renderer2d() {
			delete m_batch;
			m_batch = nullptr;
		}

		void renderer2d::draw() {
			// nothing to draw
			if (m_cameras.size() == 0 || m_sprites.size() == 0)
				return;

			// select a camera
			if (m_mainCamera == nullptr) {
				m_mainCamera = *(m_cameras.begin());
			}

			m_batch->clear();
			camera2d* mainCam = m_mainCamera;

			glm::mat4 transform;

			for (auto& [index, arr] : m_sprites) {
				for (auto* sprite : arr) {
					// get data
					transform = sprite->get_transform();

					// push
					m_batch->push(
						sprite->m_spriteBounds, 
						BE_COLOR_WHITE, 
						nullptr, 
						nullptr, 
						&transform
					);
				}
			}

			m_batch->draw(mainCam->get_view());
		}

		camera2d* renderer2d::get_main_camera() {
			return nullptr;
		}

		void renderer2d::__add_renderer(int32 index, spriterenderer* renderer) {
			if (!m_sprites.contains(index)) {
				m_sprites.emplace(index, std::vector<spriterenderer*>()); // insert new vector at index
			}
			m_sprites[index].push_back(renderer);
		}

		void renderer2d::__remove_renderer(int32 index, spriterenderer* renderer) {
			if (m_sprites.contains(index)) {
				auto& arr = m_sprites[index];
				for (size_t i = 0; i < arr.size(); i++) {
					if (arr[i] == renderer) {
						arr.erase(arr.begin() + i);
						if (arr.size() == 0) m_sprites.erase(index); // just get rid of the empty index
						return;
					}
				}
			}
		}

		void renderer2d::__add_camera2d(camera2d* camera) {
			m_cameras.insert(camera);
		}

		void renderer2d::__remove_camera2d(camera2d* camera) {
			m_cameras.erase(camera);
		}

		void renderer2d::__set_main_camera2d(camera2d* camera) {
			if (m_cameras.contains(camera)) {
				m_mainCamera = camera;
			}
		}

	}
}
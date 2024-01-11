#ifndef SCENE_HPP
#define SCENE_HPP
#include <be\node.hpp>
#include <be\core\canvas.hpp>

class scene : public be::node {
public:
	virtual ~scene() = 0 {}

	virtual void step(float delta) = 0;
	virtual void draw(be::canvas* canvas) = 0;
};

#endif // !SCENE_HPP
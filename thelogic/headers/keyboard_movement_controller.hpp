#pragma once


#define GLM_ENABLE_EXPERIMENTAL

#include "the_game_object.hpp"
#include "the_window.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace the
{
	class KeyboardMovementController
	{
	public:
		struct KeyMappings
		{
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int close = GLFW_KEY_ESCAPE;
		};

        void moveInPlaneXZ(GLFWwindow* window, float dt, TheGameObject &gameObject, double oMouseX, double oMouseY);
        bool mousecontrol;

        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.0f };
	};
}

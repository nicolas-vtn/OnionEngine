#pragma once

#include <glm/glm.hpp>

namespace Onion::Rendering {

	struct Transform {

		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f }; // Euler angles in degrees
		glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 GetModelMatrix() const {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, Position);
			model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, Scale);
			return model;
		}
	};

} // namespace Onion::Rendering
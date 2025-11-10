#include "camera.hpp"

using namespace Onion::Rendering;

Camera::Camera(glm::vec3 startPosition, int screenWidth, int screenHeight)
	: Position(startPosition)
	, Front(glm::vec3(0.0f, 0.0f, -1.0f))
	, Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, Yaw(-90.0f)
	, Pitch(0.0f) {
	SetAspectRatio(static_cast<float>(screenWidth) / static_cast<float>(screenHeight));
	UpdateProjectionMatrix();
}

Camera::~Camera() {
	// Destructor implementation
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::UpdateFront() {
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
}
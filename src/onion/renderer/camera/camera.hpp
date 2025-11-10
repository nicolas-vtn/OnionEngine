#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Onion::Rendering {
	class Camera {
	public:
		Camera(glm::vec3 startPosition, int screenWidth, int screenHeight);
		~Camera();

		glm::mat4 GetViewMatrix() const;

		// Positions
	private:
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		float Yaw;
		float Pitch;

	private:
		float FovY = 45.0f;				  // Field of view in degrees
		float AspectRatio = 16.0f / 9.0f; // Aspect ratio
		float NearPlane = 0.1f;			  // Near clipping plane
		float FarPlane = 2000.f;		  // Far clipping plane

	private:
		void UpdateFront();

		glm::mat4 m_Projection = glm::perspective(glm::radians(FovY), AspectRatio, NearPlane, FarPlane);
		bool m_UpdatedProjectionMatrix = false;

		// Getters
	public:
		glm::vec3 GetPosition() const {
			return Position;
		}
		void SetPosition(const glm::vec3& position) {
			Position = position;
		}
		glm::vec3 GetFront() const {
			return Front;
		}
		void SetFront(const glm::vec3& front) {
			Front = front;
		}
		glm::vec3 GetUp() const {
			return Up;
		}
		float GetYaw() const {
			return Yaw;
		}
		void SetYaw(float yaw) {
			Yaw = yaw;
			UpdateFront();
		}
		float GetPitch() const {
			return Pitch;
		}
		void SetPitch(float pitch) {
			Pitch = pitch;
			UpdateFront();
		}

		glm::mat4 GetProjectionMatrix() const {
			return m_Projection;
		}

		void UpdateProjectionMatrix() {
			m_Projection = glm::perspective(glm::radians(FovY), AspectRatio, NearPlane, FarPlane);
		}

		void SetAspectRatio(float aspectRatio) {
			AspectRatio = aspectRatio;
			m_UpdatedProjectionMatrix = true;
			UpdateProjectionMatrix();
		}

		void SetFovY(float fovY) {
			FovY = fovY;
			m_UpdatedProjectionMatrix = true;
			UpdateProjectionMatrix();
		}

		float GetAspectRatio() const {
			return AspectRatio;
		}

		bool IsProjectionMatrixUpdated() const {
			return m_UpdatedProjectionMatrix;
		}

		void ResetProjectionMatrixUpdateFlag() {
			m_UpdatedProjectionMatrix = false;
		}

		float GetFovY() const {
			return FovY;
		}
	};
} // namespace Onion::Rendering

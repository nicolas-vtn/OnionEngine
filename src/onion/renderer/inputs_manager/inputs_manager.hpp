#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <shared_mutex>

namespace Onion::Rendering {

	struct InputsSnapshot {
		bool FramebufferResized = false;

		bool MouseCaptureEnabled = true;
		float MouseXoffset = 0.f;
		float MouseYoffset = 0.f;

		bool MouseScrollOffsetChanged = false;

		bool MouseLeftButton = false;
		bool MouseRightButton = false;

		bool KeyW = false;
		bool KeyS = false;
		bool KeyA = false;
		bool KeyD = false;
		bool KeySpace = false;
		bool KeyEscape = false;
		bool KeyLShift = false;
		bool KeyLCtrl = false;

		bool KeyPlus = false;
		bool KeyMinus = false;

		bool KeySpaceDouble = false;
	};

	class InputsManager {
	public:
		InputsManager() = default;
		~InputsManager() = default;

		InputsManager(const InputsManager&) = delete;
		InputsManager& operator=(const InputsManager&) = delete;
		InputsManager(InputsManager&&) = delete;
		InputsManager& operator=(InputsManager&&) = delete;

		void Init(GLFWwindow* window);

		void PoolInputs();
		InputsSnapshot GetInputsSnapshot() const;

		void SetMouseCaptureEnabled(bool enabled);
		bool IsMouseCaptureEnabled() const;

		void GetFramebufferSize(int& width, int& height);
		void GetMouseScrollOffset(float& xoffset, float& yoffset);

	private:
		class KeyInputControl {
		public:
			KeyInputControl() = default;
			~KeyInputControl() = default;

			void Update(bool isKeyDown);

			bool IsPressed() const;
			bool IsDoublePressed() const;

			double KeyRepeatDelay = 0.6f;	 // Delay before repeating the key press
			double KeyRepeatInterval = 0.4f; // Interval between repeated key presses

			double DoublePressDelay = 0.5f; // Delay for double key press detection
		private:
			// Previous state
			bool m_WasDown = false;

			// For Repeated Key Presses
			bool m_IsPressed = false;
			bool m_IsHeld = false;
			bool m_FirstDelay = true;
			double m_LastPressedTime = 0.0;

			// For Double Key Presses
			bool m_IsDoublePressed = false;
			double m_LastPressedTimeDouble = 0.0;
		};

	private:
		GLFWwindow* m_Window = nullptr;
		double m_GlfwTime = 0.f;

		// Internal States
	private:
		bool m_MouseCaptureEnabled = true;
		bool m_FirstMouse = true;
		float m_MouseLastX = 0.f;
		float m_MouseLastY = 0.f;
		float m_MouseXoffset = 0.f;
		float m_MouseYoffset = 0.f;

		mutable std::shared_mutex m_MutexScrollOffset;
		bool m_MouseScrollOffsetChanged = false;
		float m_MouseScrollXoffset = 0.f;
		float m_MouseScrollYoffset = 0.f;

		KeyInputControl m_MouseLeftButtonControl;
		KeyInputControl m_MouseRightButtonControl;

		KeyInputControl m_KeyPlusControl;
		KeyInputControl m_KeyMinusControl;

		KeyInputControl m_KeySpaceDouble;

		bool m_KeyW = false;
		bool m_KeyS = false;
		bool m_KeyA = false;
		bool m_KeyD = false;
		bool m_KeySpace = false;
		bool m_KeyEscape = false;
		bool m_KeyLShift = false;
		bool m_KeyLCtrl = false;

		mutable std::shared_mutex m_MutexFramebufferSize;
		bool m_FramebufferResized = false;
		int m_WindowWidth = 800;
		int m_WindowHeight = 600;

		// Pool inputs
	private:
		void PoolMouseMovement();
		void PoolMouseInputs();
		void PollKeyboardInputs();

	private:
		// Callbacks
	private:
		void InitCallbacks();
		void FramebufferSizeCallback(int width, int height);
		void MouseScrollCallback(double xoffset, double yoffset);
	};
} // namespace Renderer_cpp
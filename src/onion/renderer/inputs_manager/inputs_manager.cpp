#include "inputs_manager.hpp"

using namespace Onion::Rendering;

void InputsManager::Init(GLFWwindow* window) {
	m_Window = window;
	glfwSetWindowUserPointer(window, this);

	glfwGetFramebufferSize(m_Window, &m_WindowWidth, &m_WindowHeight);

	InitCallbacks();
}

void InputsManager::PoolInputs() {
	m_GlfwTime = glfwGetTime();

	PoolMouseMovement();
	PoolMouseInputs();
	PollKeyboardInputs();
}

void InputsManager::PoolMouseMovement() {
	if (!m_MouseCaptureEnabled) {
		return;
	}

	double xpos, ypos;
	glfwGetCursorPos(m_Window, &xpos, &ypos);

	if (m_FirstMouse) {
		m_MouseLastX = static_cast<float>(xpos);
		m_MouseLastY = static_cast<float>(ypos);
		m_FirstMouse = false;
	}

	float xoffset = xpos - m_MouseLastX;
	float yoffset = m_MouseLastY - ypos; // reversed since y-coordinates range bottom to top
	m_MouseLastX = xpos;
	m_MouseLastY = ypos;

	m_MouseXoffset = xoffset;
	m_MouseYoffset = yoffset;

	// m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

void InputsManager::PoolMouseInputs() {
	m_MouseRightButtonControl.Update(glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	m_MouseLeftButtonControl.Update(glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

void InputsManager::PollKeyboardInputs() {

	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) {
		m_KeyW = true;
	}
	else {
		m_KeyW = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) {
		m_KeyS = true;
	}
	else {
		m_KeyS = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {
		m_KeyA = true;
	}
	else {
		m_KeyA = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
		m_KeyD = true;
	}
	else {
		m_KeyD = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		m_KeySpace = true;
	}
	else {
		m_KeySpace = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		m_KeyEscape = true;
	}
	else {
		m_KeyEscape = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		m_KeyLShift = true;
	}
	else {
		m_KeyLShift = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(m_Window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
		m_KeyLCtrl = true;
	}
	else {
		m_KeyLCtrl = false;
	}

	bool keyPlusPressed = glfwGetKey(m_Window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(m_Window, GLFW_KEY_KP_ADD) == GLFW_PRESS;
	m_KeyPlusControl.Update(keyPlusPressed);

	bool keyMinusPressed = glfwGetKey(m_Window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(m_Window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS;
	m_KeyMinusControl.Update(keyMinusPressed);

	bool keySpacePressed = glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS;
	m_KeySpaceDouble.Update(keySpacePressed);
}

InputsSnapshot InputsManager::GetInputsSnapshot() const {
	InputsSnapshot inputs;

	std::shared_lock lock(m_MutexFramebufferSize);

	inputs.FramebufferResized = m_FramebufferResized;

	inputs.MouseCaptureEnabled = m_MouseCaptureEnabled;
	inputs.MouseXoffset = m_MouseXoffset;
	inputs.MouseYoffset = m_MouseYoffset;

	inputs.MouseLeftButton = m_MouseLeftButtonControl.IsPressed();
	inputs.MouseRightButton = m_MouseRightButtonControl.IsPressed();

	inputs.MouseScrollOffsetChanged = m_MouseScrollOffsetChanged;

	inputs.KeyA = m_KeyA;
	inputs.KeyD = m_KeyD;
	inputs.KeyW = m_KeyW;
	inputs.KeyS = m_KeyS;
	inputs.KeySpace = m_KeySpace;
	inputs.KeyEscape = m_KeyEscape;
	inputs.KeyLShift = m_KeyLShift;
	inputs.KeyLCtrl = m_KeyLCtrl;

	inputs.KeyPlus = m_KeyPlusControl.IsPressed();
	inputs.KeyMinus = m_KeyMinusControl.IsPressed();

	inputs.KeySpaceDouble = m_KeySpaceDouble.IsDoublePressed();

	return inputs;
}

void InputsManager::InitCallbacks() {
	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		// Retrieve the user pointer
		auto* self = static_cast<InputsManager*>(glfwGetWindowUserPointer(window));
		if (self)
			self->FramebufferSizeCallback(width, height);
		});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
		// Retrieve the user pointer
		auto* self = static_cast<InputsManager*>(glfwGetWindowUserPointer(window));
		if (self)
			self->MouseScrollCallback(xoffset, yoffset);
		});
}

void InputsManager::FramebufferSizeCallback(int width, int height) {
	std::unique_lock lock(m_MutexFramebufferSize);
	m_WindowWidth = width;
	m_WindowHeight = height;
	m_FramebufferResized = true; // Set the framebuffer resized flag
}

void InputsManager::MouseScrollCallback(double xoffset, double yoffset) {
	std::unique_lock lock(m_MutexScrollOffset);
	m_MouseScrollXoffset = static_cast<float>(xoffset);
	m_MouseScrollYoffset = static_cast<float>(yoffset);
	m_MouseScrollOffsetChanged = true; // Set the mouse scroll offset changed flag
}

void InputsManager::SetMouseCaptureEnabled(bool enabled) {
	bool wasEnabled = m_MouseCaptureEnabled;
	m_MouseCaptureEnabled = enabled;
	if (enabled) {
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture mouse

		if (!wasEnabled) {
			m_FirstMouse = true; // Reset first mouse flag when enabling capture (Avoid sudden jump)
		}
	}
	else if (!enabled && wasEnabled) {
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Release mouse
	}
}

bool InputsManager::IsMouseCaptureEnabled() const {
	return m_MouseCaptureEnabled;
}

void InputsManager::GetFramebufferSize(int& width, int& height) {
	std::unique_lock lock(m_MutexFramebufferSize);
	width = m_WindowWidth;
	height = m_WindowHeight;
	m_FramebufferResized = false; // Reset the framebuffer resized flag after getting the size
}

void InputsManager::GetMouseScrollOffset(float& xoffset, float& yoffset) {
	xoffset = m_MouseScrollXoffset;
	yoffset = m_MouseScrollYoffset;
	m_MouseScrollOffsetChanged = false; // Reset the framebuffer resized flag after getting the size
}

void InputsManager::KeyInputControl::Update(bool isKeyDown) {
	double now = glfwGetTime();

	// Handle Repeated Key presses
	if (isKeyDown) {
		if (!m_WasDown) {
			// Key was just pressed this frame
			m_IsPressed = true; // This frame, report as pressed
			m_IsHeld = true;
			m_FirstDelay = true;
			m_LastPressedTime = now;
		}
		else if (m_IsHeld) {
			if (m_FirstDelay) {
				if (now - m_LastPressedTime >= KeyRepeatDelay) {
					m_IsPressed = true; // First repeat
					m_FirstDelay = false;
					m_LastPressedTime = now;
				}
				else {
					m_IsPressed = false; // Wait for initial delay
				}
			}
			else {
				if (now - m_LastPressedTime >= KeyRepeatInterval) {
					m_IsPressed = true; // Subsequent repeats
					m_LastPressedTime = now;
				}
				else {
					m_IsPressed = false; // Not yet time for another repeat
				}
			}
		}
	}
	else {
		// Key is up
		m_IsPressed = false;
		m_IsHeld = false;
		m_FirstDelay = true;
		m_LastPressedTime = 0.0;
	}

	// Handle Double Key presses
	m_IsDoublePressed = false; // Reset double pressed state
	if (isKeyDown && !m_WasDown) {
		// Key was just pressed this frame
		// Check for double press
		if (m_LastPressedTimeDouble != 0.0 && (now - m_LastPressedTimeDouble <= DoublePressDelay)) {
			m_IsDoublePressed = true;	   // Double press detected
			m_LastPressedTimeDouble = 0.0; // Reset for next detection
		}
		else {
			m_LastPressedTimeDouble = now; // Update last pressed time
		}
	}

	m_WasDown = isKeyDown;
}

bool InputsManager::KeyInputControl::IsPressed() const {
	return m_IsPressed;
}

bool InputsManager::KeyInputControl::IsDoublePressed() const {
	return m_IsDoublePressed;
}

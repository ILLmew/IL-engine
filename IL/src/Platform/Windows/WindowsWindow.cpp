#include "ilpch.h"
#include "WindowsWindow.h"

#include "IL/Events/KeyEvent.h"
#include "IL/Events/MouseEvent.h"
#include "IL/Events/ApplicationEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "GLFW/glfw3.h"

namespace IL
{

	static bool m_GLFWInitialized = false;

	static void ErrorCallback(int errorCode, const char* description)
	{
		IL_CORE_ASSERT("GLFW ERROR ({0}): {1}", errorCode, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		IL_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		IL_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		IL_PROFILE_FUNCTION();

		m_Data.title  = props.titles;
		m_Data.width  = props.width;
		m_Data.height = props.height;
		
		IL_CORE_INFO("Create window {0}, ({1}, {2})", props.titles, props.width, props.height);

		if (!m_GLFWInitialized)
		{
			int success = glfwInit();
			IL_CORE_ASSERT(success, "Failed to initialize GLFW!");
			// set glfwError callback
			glfwSetErrorCallback(ErrorCallback);

			m_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.width, (int)props.height, props.titles.c_str(), nullptr, nullptr);

		// create context for OpenGL
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// set GLFW callback function
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.width = width;
				data.height = height;

				WindowResizeEvent e(width, height);
				data.EventCallbackFunc(e);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent e((float)xPos, (float)yPos);
				data.EventCallbackFunc(e);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent e;
				data.EventCallbackFunc(e);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent e((float)xOffset, (float)yOffset);
				data.EventCallbackFunc(e);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallbackFunc(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent e(key);
					data.EventCallbackFunc(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					data.EventCallbackFunc(e);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent e(keycode);
				data.EventCallbackFunc(e);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallbackFunc(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallbackFunc(e);
					break;
				}
				}
			});
	}

	void WindowsWindow::Shutdown()
	{
		IL_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		IL_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enable)
	{
		IL_PROFILE_FUNCTION();

		if (enable)
			glfwSwapInterval(1);
		else 
			glfwSwapInterval(0);

		m_Data.VSync = enable;
	}
}

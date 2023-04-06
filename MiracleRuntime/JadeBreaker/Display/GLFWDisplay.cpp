#include "GLFWDisplay.h"

namespace Sherphy 
{
	void GLFWDisplay::init(uint32_t width, uint32_t height) 
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(width, height, "Sherphy_Engine", nullptr, nullptr);
		SHERPHY_EXCEPTION_IF_FALSE(m_window, "GLFW Display Window Init Faild\n");
	}

	void GLFWDisplay::distroy() 
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void GLFWDisplay::tick() 
	{
		
	}
}

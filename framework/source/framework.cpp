#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "framework.h"
#include <iostream>



Framework::Framework()  : m_mainWindow()
{

}

Framework::~Framework() 
{

}

bool Framework::Initialise(const char* a_windowName, const int a_width, const int a_height) 
{
	//initialise glfw
	if (!glfwInit())
	{
		//if we cannot init GLFW exit false
		return false;
	}

	GLFWwindow* window = glfwCreateWindow(a_width, a_height, a_windowName, nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		return false;
	}

	m_mainWindow = a_windowName;
	m_applicationWindows[a_windowName] = ((void*)window);
	
	//make window context current
	glfwMakeContextCurrent(window);

	//initialise glad
	if (!gladLoadGL())
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	int revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

	std::cout << "OpenGL Version Supported: " << major << "." << minor << "." << revision << std::endl;

	return true;
}

void Framework::Update()
{
	//cast window back
	GLFWwindow* window = (GLFWwindow*)m_applicationWindows[m_mainWindow.c_str()];
	glClearColor(0.6f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Framework::Destroy() 
{
	//loop through all our windows
	for (auto iter = m_applicationWindows.begin(); iter != m_applicationWindows.end(); ++iter) 
	{
		GLFWwindow* window = (GLFWwindow*)(iter->second);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

bool Framework::CreateWindow(std::string& a_windowName, const int a_width, const int a_height) 
{
	GLFWwindow* window = glfwCreateWindow(a_width, a_height, a_windowName.c_str(), nullptr, nullptr);

	if (!window)
	{
		return false;
	}
	//every time Creat window is called we cast it to this void and push it back
	m_applicationWindows[a_windowName] = ((void*)window);
	return m_applicationWindows.size() - 1;
}

bool Framework::DestroyWindow(std::string& a_window)
{
	auto iter = m_applicationWindows.find(a_window);
	if (iter != m_applicationWindows.end()) 
	{
		GLFWwindow* window = (GLFWwindow*)(iter->second);
		glfwDestroyWindow(window);
		m_applicationWindows.erase(iter);
		return true;
	}
	return false;
}

bool Framework::MainwindowActive() 
{
	GLFWwindow* window = (GLFWwindow*)(m_applicationWindows[m_mainWindow.c_str()]);

	return !glfwWindowShouldClose(window);
}
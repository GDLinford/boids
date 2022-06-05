#pragma once

#include <vector>
#include <map>
#include <string>

class Framework 
{
public:
	Framework();
	~Framework();

	bool Initialise(const char* a_windowName, const int a_width, const int a_height);
	void Update();
	void Destroy();

	bool CreateWindow(std::string& a_windowName, const int a_width, const int a_height);
	bool DestroyWindow(std::string& a_window);

	bool MainwindowActive();

private:
	std::string m_mainWindow;
	std::map<std::string, void*> m_applicationWindows;

};
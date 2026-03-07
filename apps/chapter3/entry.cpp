#include "entry.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace SJH::Chapter3
{

	bool my_application::init(int width, int height, const char *title)
	{
		if (!glfwInit())
		{
			std::cerr << "GLFW 초기화 실패" << std::endl;
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window)
		{
			std::cerr << "윈도우 생성 실패" << std::endl;
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(window);

		if (gl3wInit())
		{
			std::cerr << "gl3w 초기화 실패" << std::endl;
			glfwTerminate();
			return false;
		}

		std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);

		return true;
	}

	void my_application::run()
	{
		while (!glfwWindowShouldClose(window))
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);

			render(glfwGetTime());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void my_application::shutdown()
	{
		glfwTerminate();
		window = nullptr;
	}

	void my_application::render(double /*currentTime*/)
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

} // namespace SJH::Chapter3

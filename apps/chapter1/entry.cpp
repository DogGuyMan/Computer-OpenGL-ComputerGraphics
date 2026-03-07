#include "entry.h"

#ifdef _WIN32
// ===================== Windows: freeglut =====================
#include <GL/glut.h>
#include <iostream>

namespace SJH::Chapter1
{
	static my_application *s_instance = nullptr;

	static void display_callback()
	{
		if (s_instance)
		{
			s_instance->render();
			glutSwapBuffers();
		}
	}

	static void reshape_callback(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	static void keyboard_callback(unsigned char key, int /*x*/, int /*y*/)
	{
		if (key == 27) // ESC
			exit(0);
	}

	bool my_application::init(int argc, char *argv[],
							  int width, int height, const char *title)
	{
		s_instance = this;

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(width, height);

		window = glutCreateWindow(title);
		if (window <= 0)
		{
			std::cerr << "윈도우 생성 실패" << std::endl;
			return false;
		}

		std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

		glutDisplayFunc(display_callback);
		glutReshapeFunc(reshape_callback);
		glutKeyboardFunc(keyboard_callback);
		glutIdleFunc(display_callback);

		return true;
	}

	void my_application::run()
	{
		glutMainLoop();
	}

	void my_application::render()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

} // namespace SJH::Chapter1

#else
// ===================== macOS/Linux: GLFW + gl3w =====================
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace SJH::Chapter1
{
	bool my_application::init(int /*argc*/, char * /*argv*/[],
							  int width, int height, const char *title)
	{
		if (!glfwInit())
		{
			std::cerr << "GLFW 초기화 실패" << std::endl;
			return false;
		}

		// OpenGL 3.3 Core Profile
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

			render();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glfwTerminate();
	}

	void my_application::render()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

} // namespace SJH::Chapter1

#endif

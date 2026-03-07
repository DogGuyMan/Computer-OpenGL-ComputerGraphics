#ifndef __CHAPTER_1_ENTRY_H__
#define __CHAPTER_1_ENTRY_H__

#ifdef _WIN32
// Windows: freeglut
#else
// macOS/Linux: GLFW
struct GLFWwindow;
#endif

namespace SJH::Chapter1
{

	class my_application
	{
	public:
		bool init(int argc, char *argv[],
				  int width = 800, int height = 600, const char *title = "Chapter1");

		void run();

		virtual void render();

		virtual ~my_application() = default;

	protected:
#ifdef _WIN32
		int window = 0;
#else
		GLFWwindow *window = nullptr;
#endif
	};

} // namespace SJH::Chapter1

#endif // __CHAPTER_1_ENTRY_H__

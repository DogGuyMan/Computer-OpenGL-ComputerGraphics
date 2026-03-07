#ifndef __CHAPTER_2_ENTRY_H__
#define __CHAPTER_2_ENTRY_H__

struct GLFWwindow;

namespace SJH::Chapter2
{

class my_application
{
public:
	bool init(int width = 800, int height = 600, const char *title = "Chapter2");
	void run();
	void shutdown();
	virtual void render(double currentTime);
	virtual ~my_application() = default;

protected:
	GLFWwindow *window = nullptr;
};

} // namespace SJH::Chapter2

#endif // __CHAPTER_2_ENTRY_H__

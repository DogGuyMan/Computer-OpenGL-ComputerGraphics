#ifndef __CHAPTER_3_ENTRY_H__
#define __CHAPTER_3_ENTRY_H__

struct GLFWwindow;

namespace SJH::Chapter3
{

class my_application
{
public:
	bool init(int width = 800, int height = 600, const char *title = "Chapter3");
	void run();
	void shutdown();
	virtual void render(double currentTime);
	virtual ~my_application() = default;

protected:
	GLFWwindow *window = nullptr;
};

} // namespace SJH::Chapter3

#endif // __CHAPTER_3_ENTRY_H__

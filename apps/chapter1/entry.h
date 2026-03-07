#ifndef __CHAPTER_1_ENTRY_H__
#define __CHAPTER_1_ENTRY_H__

struct GLFWwindow;

namespace SJH::Chapter1
{

class my_application
{
public:
	// 애플리케이션 초기화 (윈도우 생성, OpenGL 컨텍스트)
	bool init(int width = 800, int height = 600, const char *title = "Chapter1");

	// 메인 루프 실행
	void run();

	// 정리
	void shutdown();

	// 매 프레임 렌더링 (오버라이드 용도)
	virtual void render(double currentTime);

	virtual ~my_application() = default;

protected:
	GLFWwindow *window = nullptr;
};

} // namespace SJH::Chapter1

#endif // __CHAPTER_1_ENTRY_H__

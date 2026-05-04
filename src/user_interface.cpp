#include "user_interface.h"
#include "glm/gtc/type_ptr.hpp"
#include <imgui.h>
#include <imgui_impl_glut.h>

void Metahuman::InitImgui()
{
	IMGUI_CHECKVERSION();
	// ImGui 컨텍스트 초기화
	// ImGui 내부 상태(윈도우, 위젯, 폰트 등) 메모리 할당. 
	// 	ImGui의 "뇌"를 만드는 것
	// 만약 없다면 	모든 ImGui 호출이 nullptr 접근 -> 크래시
	ImGui::CreateContext();
	
	// ImGuiIO &io = ImGui::GetIO();
	// (void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 키보드 조작 활성화

	// ImGui 스타일 설정
	ImGui::StyleColorsDark();

	// 플랫폼/렌더러 백엔드 초기화 
	// "입력을 어디서 받을지" 연결. 
	// GLUT 윈도우 크기, 마우스/키보드 이벤트를 ImGui가 이해할 수 있게 변환하는 레이어
	// 이게 없다면 ImGui가 윈도우 크기를 모름 -> UI가 좌표 0,0에 크기 0으로 렌더 (안 보이거나 깨짐)
	ImGui_ImplGLUT_Init(); 

	// ImGui의 드로우 명령을 OpenGL 2.x의 glBegin/glEnd 호출로 변환하는 레이어
	// 이게 없으면 ImGui::Render() 후 RenderDrawData()가 아무것도 못 그림 (OpenGL 상태 미설정)
	ImGui_ImplOpenGL2_Init();


	// GLUT 핸들러 설치
	// io.WantCaptureMouse가 true면 마우스 입력을 앱에 전달하지 말 것. 
	// io.WantCaptureKeyboard가 true면 키보드 입력을 앱에 전달하지 말 것. 
	// 일반적으로 모든 입력을 ImGui에 넘기고, 이 두 플래그로 앱 전달 여부를 결정하면 됨.
	// 사실상 하는 작업은 아래와 같다.
	// 	ImGui가 GLUT 콜백을 자기 것으로 전부 교체
	// 	glutReshapeFunc(ImGui_ImplGLUT_ReshapeFunc);
	// 	glutDisplayFunc(ImGui_ImplGLUT_DisplayFunc);    // <- 빈 함수
	// 	glutKeyboardFunc(ImGui_ImplGLUT_KeyboardFunc);
	// 	glutMouseFunc(ImGui_ImplGLUT_MouseFunc);
	// 	glutMotionFunc(ImGui_ImplGLUT_MotionFunc);
	// 	... 등등
	// ❌ glut 콜백은 우리가 직접 주입하는것이므로, 사용하지 말자! ❌ ImGui_ImplGLUT_InstallFuncs();
}

void Metahuman::UIBeginFrame()
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	ImGui::NewFrame();
}

void Metahuman::UIEndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void Metahuman::UITransformPanel(const char* label, Metahuman::PODTransform& form)
{
	ImGui::Begin(label);
	ImGui::DragFloat3("Position", glm::value_ptr(form.translate));
	ImGui::DragFloat3("Rotate(deg)", glm::value_ptr(form.eulerDeg));
	ImGui::DragFloat3("Scale", glm::value_ptr(form.scale));
	ImGui::End();
}

void Metahuman::UIUVPanel(const char* label, Metahuman::UVTransform& uv)
{
	ImGui::Begin(label);
	ImGui::DragFloat2("UV Offset", glm::value_ptr(uv.offset), 0.01f);
	ImGui::DragFloat2("UV Scale",  glm::value_ptr(uv.scale),  0.01f);
	ImGui::DragFloat ("UV Rotate(deg)", &uv.rotationDeg);
	ImGui::End();
}

void Metahuman::UIReshape(int w, int h) 
{
	ImGui_ImplGLUT_ReshapeFunc(w, h);
}

void Metahuman::UIMouse(int button, int state, int x, int y) 
{
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void Metahuman::UIMotion(int x, int y) 
{
	ImGui_ImplGLUT_MotionFunc(x, y);
}

void Metahuman::UIKeyboardInput(unsigned char key, int x, int y) {
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}
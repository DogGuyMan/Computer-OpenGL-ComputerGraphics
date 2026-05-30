#include "user_interface.h"
#include "constants.h"
#include "glm/gtc/type_ptr.hpp"
#include <cstdio>
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

	ImGuiIO &io = ImGui::GetIO();
	// 드래그 위젯(DragFloat 등)에서 클릭만(드래그X)으로 InputText 모드 진입.
	// Ctrl+Click 동작은 그대로 유지됨.
	io.ConfigDragClickToInputText = true;
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

void Metahuman::UITransformPanel(const char* label, Metahuman::ITransformable& transform,
                                 int& modelIndex, const std::vector<const char*>& modelLabels)
{
	ImGui::Begin(label);

	const int modelCount = (int)modelLabels.size();
	if (modelCount > 0) {
		if (modelIndex < 0) modelIndex = 0;
		if (modelIndex >= modelCount) modelIndex = modelCount - 1;

		const char* preview = modelLabels[(size_t)modelIndex];
		if (ImGui::BeginCombo(Metahuman::UI::LABEL::TARGET_MODEL, preview)) {
			for (int i = 0; i < modelCount; ++i) {
				const char* item = modelLabels[(size_t)i];
				const bool selected = (i == modelIndex);
				if (ImGui::Selectable(item, selected))
					modelIndex = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
	}

	// 인터페이스로 현재 값을 복사해 와서 편집 : 변경 시에만 다시 Set.
	Metahuman::TransformValue form = transform.GetPODTransform();
	bool changed = false;
	changed |= ImGui::DragFloat3(Metahuman::UI::LABEL::POSITION,   glm::value_ptr(form.translate), 0.05f);
	changed |= ImGui::DragFloat3(Metahuman::UI::LABEL::ROTATE_DEG, glm::value_ptr(form.eulerDeg),  1.0f);
	changed |= ImGui::DragFloat3(Metahuman::UI::LABEL::SCALE,      glm::value_ptr(form.scale),     0.05f);
	if (changed)
		transform.SetTransform(form);

	ImGui::End();
}

void Metahuman::UIUVPanel(const char* label, Metahuman::IUVTransformable& uvt)
{
	ImGui::Begin(label);

	// 인터페이스로 현재 값을 복사해 와서 편집 : 변경 시에만 다시 Set.
	Metahuman::UVValue uv = uvt.GetUV();
	bool changed = false;
	changed |= ImGui::DragFloat2(Metahuman::UI::LABEL::UV_OFFSET,     glm::value_ptr(uv.offset), 0.05f);
	changed |= ImGui::DragFloat2(Metahuman::UI::LABEL::UV_SCALE,      glm::value_ptr(uv.scale),  0.05f);
	changed |= ImGui::DragFloat (Metahuman::UI::LABEL::UV_ROTATE_DEG, &uv.rotationDeg,           1.0f);
	if (changed)
		uvt.SetUV(uv);

	ImGui::End();
}

void Metahuman::UIParametricPanel(const char* label, Metahuman::IParametricTransformable& geo)
{
	ImGui::Begin(label);

	// 인터페이스로 현재 값을 복사해 와서 편집 : 변경 시에만 다시 Set.
	Metahuman::ParametricValue p = geo.GetParametricParams();
	bool changed = false;

	// u/v 범위 : uStart 등은 double. DragScalar로 임시 float 없이 직접 편집.
	ImGui::TextUnformatted(Metahuman::UI::LABEL::RANGE);
	changed |= ImGui::DragScalar(Metahuman::UI::LABEL::U_START, ImGuiDataType_Double, &p.uStart, 0.01f, nullptr, nullptr, "%.3f");
	changed |= ImGui::DragScalar(Metahuman::UI::LABEL::U_END,   ImGuiDataType_Double, &p.uEnd,   0.01f, nullptr, nullptr, "%.3f");
	changed |= ImGui::DragScalar(Metahuman::UI::LABEL::V_START, ImGuiDataType_Double, &p.vStart, 0.01f, nullptr, nullptr, "%.3f");
	changed |= ImGui::DragScalar(Metahuman::UI::LABEL::V_END,   ImGuiDataType_Double, &p.vEnd,   0.01f, nullptr, nullptr, "%.3f");

	// 해상도 : uRes/vRes는 size_t. ImGui에 size_t 위젯이 없어 int 임시값으로 편집 후 클램프.
	ImGui::Separator();
	ImGui::TextUnformatted(Metahuman::UI::LABEL::RESOLUTION);
	int uResI = static_cast<int>(p.uRes);
	int vResI = static_cast<int>(p.vRes);
	bool resChanged = false;
	resChanged |= ImGui::DragInt(Metahuman::UI::LABEL::U_RES, &uResI, 0.5f, 1, 256);
	resChanged |= ImGui::DragInt(Metahuman::UI::LABEL::V_RES, &vResI, 0.5f, 1, 256);
	if (resChanged)
	{
		// 0이면 build()에서 0 나눗셈 -> 최소 1로 클램프
		p.uRes = static_cast<size_t>(uResI < 1 ? 1 : uResI);
		p.vRes = static_cast<size_t>(vResI < 1 ? 1 : vResI);
		changed = true;
	}

	// 라이브 재생성 : 값이 바뀐 프레임에만 Set (내부에서 build() 호출)
	if (changed)
		geo.SetParametricParams(p);

	ImGui::End();
}

void Metahuman::UIHyperboloidPanel(const char* label, Metahuman::IHyperboloidTransformable& geo)
{
	ImGui::Begin(label);

	// 인터페이스로 현재 값을 복사해 와서 편집 : 변경 시에만 다시 Set.
	Metahuman::HyperboloidValue p = geo.GetHyperboloidParams();
	bool changed = false;

	// Paul Bourke 1-sheet hyperboloid 형상 파라미터
	ImGui::TextUnformatted(Metahuman::UI::LABEL::HYPERBOLOID_HEADER);
	changed |= ImGui::DragFloat(Metahuman::UI::LABEL::RADIUS, &p.radius, 0.01f, 0.0f, 100.0f, "%.3f");
	changed |= ImGui::DragFloat(Metahuman::UI::LABEL::HEIGHT, &p.height, 0.01f, 0.0f, 100.0f, "%.3f");
	// shape(d): 0이면 이중원뿔, 클수록 원기둥에 수렴
	changed |= ImGui::DragFloat(Metahuman::UI::LABEL::SHAPE,  &p.shape,  0.01f, 0.0f, 100.0f, "%.3f");

	// 라이브 재생성 : 값이 바뀐 프레임에만 Set (내부에서 build() 호출)
	if (changed)
		geo.SetHyperboloidParams(p);

	ImGui::End();
}

void Metahuman::UILightingPanel(const char* label, Metahuman::LightingValue& lighting)
{
	ImGui::Begin(label);

	ImGui::TextUnformatted("Sun");
	ImGui::DragFloat("Theta", &lighting.sunThetaDeg, 0.5f);
	ImGui::DragFloat("Phi", &lighting.sunPhiDeg, 0.5f);
	ImGui::ColorEdit3("Ambient", lighting.sunAmbient);
	ImGui::ColorEdit3("Diffuse", lighting.sunDiffuse);
	ImGui::ColorEdit3("Specular", lighting.sunSpecular);

	ImGui::Separator();
	ImGui::TextUnformatted("Material");
	ImGui::ColorEdit3("Specular Color", lighting.materialSpecular);
	ImGui::DragFloat("Shininess", &lighting.materialShininess, 1.0f, 0.0f, 128.0f);

	ImGui::Separator();
	ImGui::TextUnformatted("Point Light");
	ImGui::ColorEdit3("Point Diffuse", lighting.pointDiffuse);
	ImGui::ColorEdit3("Point Specular", lighting.pointSpecular);
	ImGui::DragFloat("Point Radius", &lighting.pointRadius, 0.05f, 0.0f, 20.0f);
	ImGui::DragFloat("Point Height", &lighting.pointHeight, 0.05f, -10.0f, 10.0f);
	ImGui::DragFloat("Angular Speed", &lighting.pointAngularSpeed, 0.05f, 0.0f, 10.0f);

	ImGui::End();
}

bool Metahuman::UIModelAddPanel(const char* label, const char* const* modelTypes, int modelTypeCount,
                                int& selectedTypeIndex, int& id)
{
	bool addRequested = false;
	ImGui::Begin(label);

	if (modelTypeCount > 0) {
		if (selectedTypeIndex < 0) selectedTypeIndex = 0;
		if (selectedTypeIndex >= modelTypeCount) selectedTypeIndex = modelTypeCount - 1;

		if (ImGui::BeginCombo(Metahuman::UI::LABEL::TYPE, modelTypes[selectedTypeIndex])) {
			for (int i = 0; i < modelTypeCount; ++i) {
				const bool selected = (i == selectedTypeIndex);
				if (ImGui::Selectable(modelTypes[i], selected))
					selectedTypeIndex = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	ImGui::InputInt(Metahuman::UI::LABEL::ID, &id);
	addRequested = ImGui::Button(Metahuman::UI::LABEL::ADD_MODEL);
	ImGui::End();
	return addRequested;
}

bool Metahuman::UIScenePanel(const char* label, const char* savePath, int saveStatus)
{
	bool saveRequested = false;
	ImGui::Begin(label);
	ImGui::Text("Path: %s", savePath);
	saveRequested = ImGui::Button(Metahuman::UI::LABEL::SAVE_JSON);
	if (saveStatus == 1)
		ImGui::TextUnformatted(Metahuman::UI::LABEL::SAVE_COMPLETE);
	else if (saveStatus == 0)
		ImGui::TextUnformatted(Metahuman::UI::LABEL::SAVE_FAILED);
	ImGui::End();
	return saveRequested;
}

void Metahuman::UIReshape(int w, int h) 
{
	ImGui_ImplGLUT_ReshapeFunc(w, h);
}

void Metahuman::UIMouse(int button, int state, int x, int y) 
{
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);
}

void Metahuman::UIMouseWheel(int button, int dir, int x, int y)
{
	#ifndef __APPLE__
	ImGui_ImplGLUT_MouseWheelFunc(button, dir, x, y);
	#endif
}

void Metahuman::UIMotion(int x, int y) 
{
	ImGui_ImplGLUT_MotionFunc(x, y);
}

void Metahuman::UIKeyboardInput(unsigned char key, int x, int y) {
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
}

void Metahuman::UIKeyboardUpInput(unsigned char key, int x, int y) {
	ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);
}

void Metahuman::UISpecialInput(int key, int x, int y) {
	ImGui_ImplGLUT_SpecialFunc(key, x, y);
}

void Metahuman::UISpecialUpInput(int key, int x, int y) {
	ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
}

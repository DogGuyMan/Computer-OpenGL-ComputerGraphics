// #ifdef _WIN32
// #define WIN32_LEAN_AND_MEAN
// #define NOMINMAX
// #include <windows.h>
// #include <GL/gl.h>
// #endif

// #ifdef __APPLE__
// #define GL_SILENCE_DEPRECATION
// #include <OpenGL/gl.h>
// #include <unistd.h>
// #endif

// #include <iostream>
// #include <cstdint>

// int main()
// {
// 	// 플랫폼 확인
// #ifdef _WIN32
// 	std::cout << "[Platform] Windows (cross-compiled from Mac)" << std::endl;

// 	// Win32 API 테스트: 콘솔 제목 변경
// 	SetConsoleTitleA("OpenGL Study - Windows Build");

// 	// Win32 API 테스트: MessageBox
// 	MessageBoxA(NULL, "Win32 API works!", "Test", MB_OK);

// 	// Win32 API 테스트: 시스템 정보
// 	SYSTEM_INFO si;
// 	GetSystemInfo(&si);
// 	std::cout << "[Win32] Processor count: " << si.dwNumberOfProcessors << std::endl;

// #elif __APPLE__
// 	std::cout << "[Platform] macOS (native build)" << std::endl;
// #endif

// 	// OpenGL 버전은 컨텍스트 생성 후에만 조회 가능
// 	// 여기서는 헤더 포함과 링크가 정상인지만 확인
// 	std::cout << "[OpenGL] GL_VERSION macro defined: " << GL_VERSION << std::endl;

// 	// 고정 크기 타입 테스트
// 	std::cout << "[Types] int32_t size: " << sizeof(int32_t) << " bytes" << std::endl;
// 	std::cout << "[Types] int64_t size: " << sizeof(int64_t) << " bytes" << std::endl;
// 	std::cout << "[Types] long size: " << sizeof(long) << " bytes" << std::endl;

// 	return 0;
// }
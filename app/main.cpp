#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <config.hpp>
#include <iostream>

int main() {
    // GLFW 초기화
    if (!glfwInit()) {
        std::cerr << "GLFW 초기화 실패" << std::endl;
        return -1;
    }

    // OpenGL 버전 힌트 (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(
        800, 600, std::string(PROJECT_NAME).c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "윈도우 생성 실패" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // gl3w 초기화 (OpenGL 함수 로딩)
    if (gl3wInit()) {
        std::cerr << "gl3w 초기화 실패" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // 뷰포트 설정
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // 렌더 루프
    while (!glfwWindowShouldClose(window)) {
        // ESC 키로 종료
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // 화면 클리어
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

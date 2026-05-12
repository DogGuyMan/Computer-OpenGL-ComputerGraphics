#ifndef __METAHUMAN_RESOURCE_MANAGEMENT_H__
#define __METAHUMAN_RESOURCE_MANAGEMENT_H__

#include <memory>
#include <string>
#include <unordered_map>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// Windows의 <GL/gl.h>는 OpenGL 1.1까지만 제공 — 1.2부터 코어인 상수 보강
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace Metahuman
{
	class Texture
	{
	  private:
		GLuint textureID = 0;
		int width = 0, height = 0, channels = 0;

	  public:
		Texture(const std::string &path, GLint internal_format, GLuint format);
		~Texture(); // glDeleteTextures의 책임

		// 복사 불능
		Texture(const Texture &) = delete;
		Texture &operator=(const Texture &) = delete;

		// 이동 연산은 항상 noexcept로 만들 수 있게 설계하라. 안 그러면 표준 컨테이너에서 손해 본다.
		Texture(Texture &&) noexcept;
		Texture &operator=(Texture &&) noexcept;

		GLuint GetTextureID() const
		{
			return textureID;
		}
		int GetWidth() const
		{
			return width;
		}
		int GetHeight() const
		{
			return height;
		}
	};

	class ResourceManagement
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

	public:
		// 소유권(ownership)과 접근(access)은 다르다.
		// Raw Pointer는 누구나 접근은 가능하다 다만, 소유권은 못가져옴
		// unique_ptr가 raw 노출을 막아주지는 못한다.
		// 	unique_ptr의 보장은 "delete를 호출하는 주체가 단 하나" 인것만을 보장한다
		//	raw 포인터 복사는 자유..
		Texture *LoadTexture(const std::string &path, GLint internal_format = GL_RGB, GLuint format = GL_RGB);

		void Clear();                    // 일괄 해제
		~ResourceManagement() = default; // Manager 소멸시 모든 Texture 소멸
	};
} // namespace Metahuman
#endif //__METAHUMAN_RESOURCE_MANAGEMENT_H__
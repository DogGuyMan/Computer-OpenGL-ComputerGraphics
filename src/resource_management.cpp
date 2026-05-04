

#include "resource_management.h"
#include "config.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

namespace Metahuman
{

	// Internal Format VS Format
	/*/*********************************************************************************
	 *
	 * internalformat
	 * 	GPU가 메모리에 텍스쳐를 어떤 채널, 비트 정밀도로 보관할지
	 * 	GPU 텍스쳐 저장 형식 (저장 정밀도가 중요)
	 * format : CPU 클라이언트 입력 데이터 형식
	 * 	image 포인터가 가르키는 픽셀이 어떤 채널 순서인지
	 *	type ( GL_UNSIGNED_BYTE, GL_FLOAT 등 입력 데이터가 중요하다 )
	 *
	 *********************************************************************************/
	Texture::Texture(const std::string &path, GLint internal_format, GLuint format)
	{
		stbi_set_flip_vertically_on_load(true);

		unsigned char *image = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!image)
		{
			std::cerr << "텍스처 로딩 실패:" << path << std::endl;
			return;
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		format = (channels == 4) ? GL_RGBA : GL_RGB;
		internal_format = (channels == 4) ? GL_RGBA : GL_RGB;
		// glPixelStorei(GL_UNPACK_ALIGNMENT, (channels == 4) ? 4 : 1);                // 정렬
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(image);
	}

	Texture::~Texture()
	{
		if (textureID != 0)
			glDeleteTextures(1, &textureID);
	}

	Texture::Texture(Texture &&other) noexcept
	    : textureID(other.textureID),
	      width(other.width), height(other.height), channels(other.channels)
	{
		other.textureID = 0;
	}

	Texture &Texture::operator=(Texture &&other) noexcept
	{
		if (this != &other)
		{
			if (textureID != 0)
				glDeleteTextures(1, &textureID);
			textureID = other.textureID;
			other.textureID = 0;
			width = other.width;
			height = other.height;
			channels = other.channels;
		}
		return *this;
	}

	Texture *ResourceManagement::LoadTexture(const std::string &path,
	                                         GLint internal_format,
	                                         GLuint format)
	{
		auto it = textures.find(path);
		if(it != textures.end())
			return it->second.get();

		auto tex = std::make_unique<Texture>(path, internal_format, format);
		if(tex->GetTextureID() == 0) return nullptr;

		auto [intertedIt, result] = textures.emplace(path, std::move(tex));
		return intertedIt->second.get();
	}
	void ResourceManagement::Clear()
	{
		textures.clear();
	}

} // namespace Metahuman

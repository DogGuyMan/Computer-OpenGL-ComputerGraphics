```cpp
void loadTexture(const char* filename, GLuint* textureID) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        printf("Failed to load texture: %s\n", filename);
        exit(1);
    }

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(image);
}
```

```cpp
// stb_image로 BMP 로딩 + OpenGL 텍스처 생성
static int LoadGLTextures()
{
	const char *files[3] = {"./image/front.bmp", "./image/back.bmp", "./image/head.bmp"};

	glGenTextures(3, texture);

	for (int i = 0; i < 3; i++) {
		int w, h, channels;
		unsigned char *data = stbi_load(files[i], &w, &h, &channels, 3); // RGB 강제
		if (!data) {
			std::fprintf(stderr, "텍스처 로딩 실패: %s\n", files[i]);
			return 0;
		}

		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return 1;
}
```

```cpp
int LoadGLTextures(const char* filename, int num)
{
	int w, h, channels;
	stbi_set_flip_vertically_on_load(1); // BMP는 bottom-to-top, OpenGL도 동일 순서 기대
	unsigned char *data = stbi_load(filename, &w, &h, &channels, 3);
	if (!data) {
		std::fprintf(stderr, "텍스처 로딩 실패: %s\n", filename);
		return 0;
	}

	glGenTextures(1, &texture[num]);
	glBindTexture(GL_TEXTURE_2D, texture[num]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	stbi_image_free(data);
	return 1;
}
```

```cpp
// ====== stb_image로 BMP 로딩 + 텍스처 생성 ======
static int loadTexture(const char *filename, GLuint tex)
{
	int w, h, channels;
	unsigned char *data = stbi_load(filename, &w, &h, &channels, 3);
	if (!data) {
		std::fprintf(stderr, "텍스처 로딩 실패: %s\n", filename);
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return 1;
}

```

### `stbi_load`
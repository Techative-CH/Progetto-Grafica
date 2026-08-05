#include "texture.h"

#include <GL/freeglut.h>

#include <vector>

Eng::Texture::Texture(const std::string& name)
	: Object{ name },
	textureId{ 0 },
	width{ 0 },
	height{ 0 }
{}

Eng::Texture::~Texture()
{
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}

bool Eng::Texture::createCheckerboard(int width, int height)
{
	if (width <= 0 || height <= 0)
		return false;

	this->width = width;
	this->height = height;

	const int channels = 3;

	std::vector<unsigned char> bitmap(
		width * height * channels
	);

	const int tileSize = 32;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			bool whiteTile =
				((x / tileSize) + (y / tileSize)) % 2 == 0;

			unsigned char color = whiteTile ? 255 : 0;

			int index = (y * width + x) * channels;

			bitmap[index] = color;
			bitmap[index + 1] = color;
			bitmap[index + 2] = color;
		}
	}

	if (textureId == 0)
		glGenTextures(1, &textureId); // Generate OpenGL id

	glBindTexture(GL_TEXTURE_2D, textureId); // Bind texture

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 1 - Doesn't use padding (we use RGB)

	// Configure wrap and filters
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		GL_REPEAT
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		GL_REPEAT
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST
	);

	// Transfer bitmap to OpenGL (RAM -> GPU)
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		width,
		height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		bitmap.data() // Pointer to first element in vector
	);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

	return true;
}

void Eng::Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void Eng::Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
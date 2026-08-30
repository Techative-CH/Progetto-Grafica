#include "texture.h"

#include <GL/freeglut.h>
#include <FreeImage.h>

#include <vector>
#include <iostream>

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

Eng::Texture::Texture(const std::string& name)
	: Object{ name },
	textureId{ 0 },
	width{ 0 },
	height{ 0 },
	anisotropy{ 1.0f }, // Disabled
	maxAnisotropy{ 1.0f },
	filter{ TextureFilter::NEAREST },
	wrap{ TextureWrap::REPEAT }
{}

Eng::Texture::~Texture()
{
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}

std::vector<unsigned char> Eng::Texture::buildCheckerboardBitmap(
	int width,
	int height,
	int tileSize
) const
{
	const int channels = 3;

	std::vector<unsigned char> bitmap(
		width * height * channels
	);

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

	return bitmap;
}

bool Eng::Texture::createCheckerboard(int width, int height)
{
	if (width <= 0 || height <= 0)
		return false;

	this->width = width;
	this->height = height;

	if (textureId == 0)
		glGenTextures(1, &textureId); // Generate OpenGL id

	bind(); // Bind texture

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 1 - Doesn't use padding (we use RGB)

	glGetFloatv(
		GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
		&maxAnisotropy
	);

	if (maxAnisotropy < 1.0f)
		maxAnisotropy = 1.0f;

	// Configure wrap and apply filters
	this->applyParameters();

	int currentWidth = width;
	int currentHeight = height;
	int currentLevel = 0;
	int currentTileSize = 32;

	while (currentWidth > 0 && currentHeight > 0)
	{
		int tileSize = currentTileSize;

		if (tileSize < 1)
			tileSize = 1;

		std::vector<unsigned char> bitmap =
			buildCheckerboardBitmap(
				currentWidth,
				currentHeight,
				tileSize
			);

		// Transfer bitmap to OpenGL (RAM -> GPU)
		glTexImage2D(
			GL_TEXTURE_2D,
			currentLevel,
			GL_RGB,
			currentWidth,
			currentHeight,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bitmap.data() // Pointer to first element in vector
		);

		currentWidth /= 2;
		currentHeight /= 2;
		currentTileSize /= 2;
		currentLevel++;
	}

	unbind(); // unbind texture

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

bool Eng::Texture::loadFromFile(const std::string& filePath)
{
	FREE_IMAGE_FORMAT format =
		FreeImage_GetFileType(filePath.c_str(), 0);

	if (format == FIF_UNKNOWN)
	{
		format =
			FreeImage_GetFIFFromFilename(
				filePath.c_str()	
			);
	}

	if (format == FIF_UNKNOWN)
	{
		std::cerr
			<< "Unknown texture format: "
			<< filePath
			<< std::endl;

		return false;
	}

	FIBITMAP* bitmap =
		FreeImage_Load(
			format,
			filePath.c_str()
		);

	if (bitmap == nullptr)
	{
		std::cerr
			<< "Unable to load texture: "
			<< filePath
			<< std::endl;

		return false;
	}

	// Required by the professor for DDS textures.
	if (format == FIF_DDS)
	{
		FreeImage_FlipVertical(bitmap);
	}

	unsigned int bpp =
		FreeImage_GetBPP(bitmap);

	if (bpp != 24 && bpp != 32)
	{
		std::cerr
			<< "Unsupported texture depth ("
			<< bpp
			<< " bpp): "
			<< filePath
			<< std::endl;

		FreeImage_Unload(bitmap);
		return false;
	}

	width =
		static_cast<int>(
			FreeImage_GetWidth(bitmap)
			);

	height =
		static_cast<int>(
			FreeImage_GetHeight(bitmap)
			);

	BYTE* bits =
		FreeImage_GetBits(bitmap);

	if (bits == nullptr ||
		width <= 0 ||
		height <= 0)
	{
		FreeImage_Unload(bitmap);
		return false;
	}

	if (textureId != 0)
	{
		glDeleteTextures(
			1,
			&textureId
		);

		textureId = 0;
	}

	glGenTextures(
		1,
		&textureId
	);

	glBindTexture(
		GL_TEXTURE_2D,
		textureId
	);

	GLenum internalFormat;
	GLenum imageFormat;

	if (bpp == 32)
	{
		internalFormat = GL_RGBA;
		imageFormat = GL_BGRA;
	}
	else
	{
		internalFormat = GL_RGB;
		imageFormat = GL_BGR;
	}

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalFormat,
		width,
		height,
		0,
		imageFormat,
		GL_UNSIGNED_BYTE,
		bits
	);

	applyParameters();

	glBindTexture(
		GL_TEXTURE_2D,
		0
	);

	FreeImage_Unload(bitmap);

	return true;
}

void Eng::Texture::render() const
{
	if (textureId == 0)
		return;

	bind();
}

void Eng::Texture::setAnisotropy(float value)
{
	if (value < 1.0f)
		value = 1.0f;

	if (value > maxAnisotropy)
		value = maxAnisotropy;

	anisotropy = value;

	if (textureId == 0)
		return;

	bind();

	glTexParameterf(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAX_ANISOTROPY_EXT,
		anisotropy
	);

	unbind();
}

float Eng::Texture::getAnisotropy() const
{
	return anisotropy;
}

float Eng::Texture::getMaxAnisotropy() const
{
	return maxAnisotropy;
}

/**
 * Configure wrap and apply filters.
 */
void Eng::Texture::applyParameters() const
{
	switch (wrap)
	{
	case TextureWrap::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;

	case TextureWrap::CLAMP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	}

	switch (filter)
	{
	case TextureFilter::NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case TextureFilter::LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case TextureFilter::NEAREST_MIPMAP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;

	case TextureFilter::BILINEAR_MIPMAP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;

	case TextureFilter::TRILINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}

	glTexParameterf(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAX_ANISOTROPY_EXT,
		anisotropy
	);
}

void Eng::Texture::setFilter(TextureFilter filter)
{
	this->filter = filter;

	if (textureId == 0)
		return;

	bind();
	applyParameters();
	unbind();
}

void Eng::Texture::setWrap(TextureWrap wrap)
{
	this->wrap = wrap;

	if (textureId == 0)
		return;

	bind();
	applyParameters();
	unbind();
}

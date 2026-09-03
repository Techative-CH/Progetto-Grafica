/**
 * @file texture.cpp
 * @brief Texture file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#include "texture.h"

#include <GL/freeglut.h>

#include <FreeImage.h>

#include <iostream>

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

/**
 * @brief Creates a texture with the specified name.
 * @param name Name of the texture
 */
Eng::Texture::Texture(const std::string& name)
	: Object{ name },
	textureId{ 0 },
	width{ 0 },
	height{ 0 },
	filter{ TextureFilter::NEAREST },
	wrap{ TextureWrap::REPEAT }
{}

/**
 * @brief Destroys the texture and releases its OpenGL resource.
 */
Eng::Texture::~Texture()
{
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}

/**
 * @brief Binds the texture to the current OpenGL texture unit.
 */
void Eng::Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

/**
 * @brief Unbinds the currently bound 2D texture.
 */
void Eng::Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Loads a texture from an image file.
 * @param filePath Path of the texture file
 * @return True if the texture was loaded successfully, false otherwise
 */
bool Eng::Texture::loadFromFile(const std::string& filePath)
{
	// Detect the image format from the file content
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filePath.c_str(), 0);

	// Try to detect the format from the file extension
	if (format == FIF_UNKNOWN)
		format = FreeImage_GetFIFFromFilename(filePath.c_str());

	if (format == FIF_UNKNOWN)
	{
		std::cerr << "[TEXTURE ERROR] Unknown texture format: " << filePath << std::endl;
		return false;
	}

	// Load the image using FreeImage
	FIBITMAP* bitmap = FreeImage_Load(format, filePath.c_str());

	if (bitmap == nullptr)
	{
		std::cerr << "[TEXTURE ERROR] Unable to load texture: " << filePath << std::endl;
		return false;
	}

	// DDS textures must be vertically flipped to work with OpenGL
	if (format == FIF_DDS)
		FreeImage_FlipVertical(bitmap);

	// Get the number of bits per pixel
	unsigned int bpp = FreeImage_GetBPP(bitmap);

	// Only 24-bit RGB and 32-bit RGBA textures are supported
	if (bpp != 24 && bpp != 32)
	{
		std::cerr << "[TEXTURE ERROR] Unsupported texture depth (" << bpp << " bpp): " << filePath << std::endl;

		FreeImage_Unload(bitmap);
		return false;
	}

	width = static_cast<int>(FreeImage_GetWidth(bitmap));
	height = static_cast<int>(FreeImage_GetHeight(bitmap));

	BYTE* bits = FreeImage_GetBits(bitmap);

	if (bits == nullptr || width <= 0 || height <= 0)
	{
		std::cerr << "[TEXTURE ERROR] Invalid texture data: " << filePath << std::endl;

		FreeImage_Unload(bitmap);
		return false;
	}

	// Delete the previous OpenGL texture if one already exists
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}

	// Create and bind the OpenGL texture
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	GLenum internalFormat;
	GLenum imageFormat;

	// FreeImage stores color channels in BGR/BGRA order
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

	// Transfer image data from RAM to the GPU
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
	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(bitmap); // When loaded to OpenGL is no longer needed

	return true;
}

/**
 * @brief Binds the texture for rendering.
 */
void Eng::Texture::render() const
{
	if (textureId == 0)
		return;

	bind();
}

/**
 * @brief Applies the configured wrapping and filtering parameters.
 */
void Eng::Texture::applyParameters() const
{
	// Configure texture wrapping
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

	// Configure texture filtering
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
}

/**
 * @brief Sets the texture filtering mode.
 * @param filter Filtering mode to apply
 */
void Eng::Texture::setFilter(TextureFilter filter)
{
	this->filter = filter;

	if (textureId == 0)
		return;

	bind();
	applyParameters();
	unbind();
}


/**
 * @brief Sets the texture wrapping mode.
 * @param wrap Wrapping mode to apply
 */
void Eng::Texture::setWrap(TextureWrap wrap)
{
	this->wrap = wrap;

	if (textureId == 0)
		return;

	bind();
	applyParameters();
	unbind();
}
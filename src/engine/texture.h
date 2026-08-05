#pragma once

#include "engineApi.h"
#include "object.h"

#include <string>
#include <vector>

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

namespace Eng 
{
	enum class TextureFilter
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP,
		BILINEAR_MIPMAP,
		TRILINEAR
	};

	enum class TextureWrap
	{
		REPEAT,
		CLAMP
	};

	class ENG_API Texture : public Object
	{
	public:
		Texture(const std::string& name);
		~Texture();

		bool createCheckerboard(int width, int height);

		void setFilter(TextureFilter filter);
		void setWrap(TextureWrap wrap);

		float getAnisotropy() const;
		float getMaxAnisotropy() const;
		void setAnisotropy(float value);

		void bind() const;
		void unbind() const;

	private:
		unsigned int textureId;
		int width;
		int height;

		float anisotropy;
		float maxAnisotropy;

		TextureFilter filter;
		TextureWrap wrap;

		std::vector<unsigned char> buildCheckerboardBitmap(
			int width,
			int height,
			int tileSize
		) const;
		void applyParameters() const;
	};
}
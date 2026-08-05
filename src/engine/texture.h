#pragma once

#include "engineApi.h"
#include "object.h"

#include <string>
#include <vector>

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

		void bind() const;
		void unbind() const;

	private:
		unsigned int textureId;
		int width;
		int height;

		TextureFilter filter;
		TextureWrap wrap;
	};
}
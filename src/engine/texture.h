#pragma once

#include "object.h"

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
		virtual ~Texture();

		bool loadFromFile(const std::string& filePath);
		void render() const;

		void setFilter(TextureFilter filter);
		void setWrap(TextureWrap wrap);

	private:
		void bind() const;
		void unbind() const;

		void applyParameters() const;

		unsigned int textureId;

		int width;
		int height;

		float anisotropy;
		float maxAnisotropy;

		TextureFilter filter;
		TextureWrap wrap;
	};
}
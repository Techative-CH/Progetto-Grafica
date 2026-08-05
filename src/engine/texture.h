#pragma once

#include "engineApi.h"
#include "object.h"

#include <string>

namespace Eng 
{
	class ENG_API Texture : public Object
	{
	public:
		Texture(const std::string& name);
		~Texture();

		bool createCheckerboard(int width, int height);
		void bind() const;
		void unbind() const;

	private:
		unsigned int textureId;
		int width;
		int height;
	};
}
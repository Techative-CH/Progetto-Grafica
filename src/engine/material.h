#pragma once

#include "engineApi.h"
#include "object.h"

namespace Eng
{
	class Texture;

	class ENG_API Material : public Object
	{
	public:
		Material(const std::string& name);
		virtual ~Material();

		void setTexture(Texture* texture);
		Texture* getTexture() const;

	private:
		Texture* texture;
	};
}
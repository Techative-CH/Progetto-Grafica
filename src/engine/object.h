#pragma once

#include "engineApi.h"
#include <string>

namespace Eng
{
	class ENG_API Object
	{
	public:
		Object(const std::string& name);
		virtual ~Object();

		const std::string& getName() const;
		void setName(const std::string& name);

	private:
		std::string name;
	};
}
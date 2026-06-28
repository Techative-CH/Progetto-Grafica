#pragma once
#include <string>

namespace Eng
{
	class Object
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
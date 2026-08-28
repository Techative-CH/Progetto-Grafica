#pragma once

#include "engineApi.h"
#include "node.h"

namespace Eng
{
	class ENG_API Light : public Node
	{
	public:
		Light(const std::string& name);
		virtual ~Light();

		void render() override;
	};
}
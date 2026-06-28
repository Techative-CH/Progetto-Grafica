#pragma once

#include "node.h"

namespace Eng
{
	class Camera : public Node
	{
	public:
		Camera(const std::string& name);
		virtual ~Camera();

		void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

	private:
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;
	};
}
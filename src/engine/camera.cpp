#include "camera.h"

Eng::Camera::Camera(const std::string& name) : 
	Node{ name },
	fov{ 45.0f },
	aspectRatio{ 1.0f },
	nearPlane{ 1.0f },
	farPlane{ 100.0f }
{}

Eng::Camera::~Camera()
{}
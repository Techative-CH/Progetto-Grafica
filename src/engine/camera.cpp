#include "camera.h"
#include "engine.h"

Eng::Camera::Camera(const std::string& name) : 
	Node{ name },
	fov{ 45.0f },
	aspectRatio{ 1.0f },
	nearPlane{ 1.0f },
	farPlane{ 100.0f }
{}

Eng::Camera::~Camera()
{}

void Eng::Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	Eng::Base::getInstance().setPerspective(fov, aspectRatio, nearPlane, farPlane);
}
/**
 * @file		engine.h
 * @brief	Graphics engine main include file
 *
 * @author	Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#pragma once
 
// Standard libraries         
#include <memory> 
#include <string>
#include <vector>
#include <chrono>

// Engine API
#include "engineApi.h"

// Library informations
#ifdef _DEBUG
   #define LIB_NAME "Bansam Graphic Engine v1.0 (debug)"
#else
   #define LIB_NAME "Bansam Graphic Engine v1.0a"
#endif
   #define LIB_VERSION 10

// Include engine API headers
#include "object.h"
#include "node.h"
#include "mesh.h"
#include "camera.h"
#include "texture.h"
#include "light.h"
#include "material.h"
#include "list.h"

namespace Eng {
	/**
	 * @brief Base engine main class. This class is a singleton.
	 */
	class ENG_API Base final
	{
	public:      

	   // Const / dest
	   Base(Base const &) = delete;
	   ~Base();

	   // Operators
	   void operator=(Base const &) = delete;

	   // Singleton
	   static Base &getInstance();

	   // Init / free
	   bool init(const char* windowTitle, int width, int height);
	   bool free();

	   // Rendering
	   void clearWindow();
	   void swapBuffers();
	   void setBackgroundColor(float r, float g, float b, float a = 1.0f);
	   void setViewport(int x, int y, int width, int height);
	   void loadIdentity();

	   // Callbacks
	   void setDisplayCallback(void (*callback)());
	   void setReshapeCallback(void (*callback)(int width, int height));
	   void setKeyboardCallback(void (*callback)(unsigned char key, int mouseX, int mouseY));
	   void setSpecialCallback(void (*callback)(int key, int mouseX, int mouseY));
	   void setTimerCallback(unsigned int millis, void (*callback)(int value), int value = 0);

	   // Main loop
	   void mainLoop();
	   void postRedisplay();
	   bool isRunning() const;

	   // Cameras
	   void addCamera(Camera* camera);
	   void setCamera(Camera* camera);
	   Camera* getCamera() const;
	   Camera* getCameraAt(unsigned int index) const;
	   const std::vector<Camera*>& getCameras() const;

	   // Rendering
	   List* buildList(Node* root);
	   void render(Node* node);
	   void render(List* list);

	   // Load OVO Model
	   Node* load(const std::string& filename);

	   // Wireframe mode
	   void setWireframe(bool enabled);
	   bool isWireframe() const;

	   // Flat shading / Gouraud
	   void setSmoothShading(bool enabled);
	   bool isSmoothShading();

	   // Render text
	   void renderText(const std::string& text, int x, int y, float r = 1.0f, float g = 1.0f, float b = 1.0f);
	   void renderFPS();

	private:
		Camera* currentCamera;
		std::vector<Camera*> cameras;

		std::chrono::time_point<std::chrono::steady_clock> lastFpsTime;
		int frameCount = 0;
		float fps = 0.0f;

		bool wireframe = false;
		bool smoothShading = false;

	   // Reserved
	   struct Reserved;
	   std::unique_ptr<Reserved> reserved;

	   // Const / dest
	   Base();

	   void calculateFPS();
	};
};


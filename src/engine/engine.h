/**
 * @file		engine.h
 * @brief	Graphics engine main include file
 *
 * @author	Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#pragma once


 
//////////////
// #INCLUDE //
//////////////

// C/C++:         
#include <memory> 

#include "engineApi.h"



/////////////
// VERSION //
/////////////

// Generic info:
#ifdef _DEBUG
   #define LIB_NAME      "My Graphics Engine v0.1a (debug)"   ///< Library credits
#else
   #define LIB_NAME      "My Graphics Engine v0.1a"   ///< Library credits
#endif
   #define LIB_VERSION   10                           ///< Library version (divide by 10)

#include "object.h"
#include "node.h"
#include "mesh.h"
#include "camera.h"
#include "texture.h"
#include "light.h"
#include "material.h"
#include "list.h"


///////////////
// NAMESPACE //
///////////////

namespace Eng {



//////////////
// #INCLUDE //
//////////////   

// You can subinclude here other headers of your engine...

///////////////////////
// MAIN ENGINE CLASS //
///////////////////////

/**
 * @brief Base engine main class. This class is a singleton.
 */
class ENG_API Base final
{
//////////
public: //
//////////	      

   // Const/dest:
   Base(Base const &) = delete;
   ~Base();

   // Operators:
   void operator=(Base const &) = delete;

   // Singleton:
   static Base &getInstance();

   // Init/free:
   bool init(const char* windowTitle, int width, int height);
   bool free();

   // Rendering:
   void clearWindow();
   void swapBuffers();
   void setBackgroundColor(float r, float g, float b, float a = 1.0f);
   void setViewport(int x, int y, int width, int height);
   void setPerspective(float fov, float aspect, float nearPlane, float farPlane);
   void loadIdentity();

   // Callbacks:
   void setDisplayCallback(void (*callback)());
   void setReshapeCallback(void (*callback)(int width, int height));
   void setKeyboardCallback(void (*callback)(unsigned char key, int mouseX, int mouseY));
   void setSpecialCallback(void (*callback)(int key, int mouseX, int mouseY));
   void setTimerCallback(unsigned int millis, void (*callback)(int value), int value = 0);

   // Main loop:
   void mainLoop();
   void postRedisplay();
   bool isRunning() const;

   // Transformations:
   void translate(float x, float y, float z);
   void rotate(float angle, float x, float y, float z);
   void scale(float x, float y, float z);

   // Primitives:
   void drawCube(float edge);

   // Rendering:
   List* buildList(Node* root);
   void render(Node* node);

///////////
private: //
///////////	

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Base();

   void buildListRecursive(Node* node, List* list);
};

}; // end of namespace Eng::


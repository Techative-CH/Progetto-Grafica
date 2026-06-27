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

// Export API:
#ifdef _WINDOWS
   // Specifies i/o linkage (VC++ spec):
   #ifdef ENGINE_EXPORTS
      #define ENG_API __declspec(dllexport)
   #else
      #define ENG_API __declspec(dllimport)
   #endif      

   // Get rid of annoying warnings:
   #pragma warning(disable : 4251) 
#else // Under linux
   #define ENG_API
#endif



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
   bool isRunning() const;

   // Callbacks:
   void setDisplayCallback(void (*callback)());
   void setKeyboardCallback(void (*callback)(unsigned char key, int mouseX, int mouseY));
   void setSpecialCallback(void (*callback)(int key, int mouseX, int mouseY));

   // Main loop:
   void mainLoop();
   void postRedisplay();


///////////
private: //
///////////	

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Base();
};

}; // end of namespace Eng::


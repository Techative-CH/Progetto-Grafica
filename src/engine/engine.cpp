/**
 * @file    engine.cpp
 * @brief	Graphics engine main file
 *
 * @author	Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */



//////////////
// #INCLUDE //
//////////////

// Main include:
#include "engine.h"

// C/C++:
#include <iostream>
#include <source_location>

// GLM
#include <glm/glm.hpp>

// FreeGLUT
#include <GL/freeglut.h>

// Callbacks
static void (*userDisplayCallback)() = nullptr;
static void (*userReshapeCallback)(int, int) = nullptr;
static void (*userKeyboardCallback)(unsigned char, int, int) = nullptr;
static void (*userSpecialCallback)(int, int, int) = nullptr;
static void (*userTimerCallback)(int) = nullptr;

static void displayCallbackBridge()
{
    if (userDisplayCallback)
        userDisplayCallback();
}

static void keyboardCallbackBridge(unsigned char key, int x, int y)
{
    if (userKeyboardCallback)
        userKeyboardCallback(key, x, y);
}

static void specialCallbackBridge(int key, int x, int y)
{
    if (userSpecialCallback)
        userSpecialCallback(key, x, y);
}

static void reshapeCallbackBridge(int width, int height)
{
    if (userReshapeCallback)
        userReshapeCallback(width, height);
}

static void timerCallbackBridge(int value)
{
    if (userTimerCallback)
        userTimerCallback(value);
}


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Base class reserved structure (using PIMPL/Bridge design pattern https://en.wikipedia.org/wiki/Opaque_pointer).
 */
struct Eng::Base::Reserved
{
   // Flags:
   bool initFlag;
   bool running;
   int windowId;


   /**
    * Constructor.
    */
   Reserved() : 
       initFlag { false },
       running { false },
       windowId { 0 }
   {}
};



////////////////////////
// BODY OF CLASS Base //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>())
{
#ifdef _DEBUG
   std::cout << "[+] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Base::~Base()
{
#ifdef _DEBUG
   std::cout << "[-] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets a reference to the (unique) singleton instance.
 * @return reference to singleton instance
 */
Eng::Base ENG_API &Eng::Base::getInstance()
{
   static Base instance;
   return instance;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init internal components.
 * @return TF
 */
bool ENG_API Eng::Base::init(const char* windowTitle, int width, int height)
{
    if (reserved->initFlag)
        return false;

    int argc = 1;
    char* argv[1] = { (char*)"engine" };

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    reserved->windowId = glutCreateWindow(windowTitle);

    glEnable(GL_DEPTH_TEST);

    reserved->initFlag = true;
    reserved->running = true;

    std::cout << "[>] " << LIB_NAME << " initialized" << std::endl;
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Free internal components.
 * @return TF
 */
bool ENG_API Eng::Base::free()
{
   // Not initialized?
   if (!reserved->initFlag)
   {
      std::cout << "ERROR: engine not initialized" << std::endl;
      return false;
   }

   // Here you can properly dispose of any allocated resource (including third-party dependencies)...

   // Done:
   std::cout << "[<] " << LIB_NAME << " deinitialized" << std::endl;
   reserved->initFlag = false;
   reserved->running = false;

   return true;
}

bool ENG_API Eng::Base::isRunning() const
{
    return reserved->running;
}

void ENG_API Eng::Base::clearWindow()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ENG_API Eng::Base::swapBuffers()
{
    glutSwapBuffers();
}

void ENG_API Eng::Base::setBackgroundColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void ENG_API Eng::Base::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void ENG_API Eng::Base::mainLoop()
{
    glutMainLoop();
}

void ENG_API Eng::Base::postRedisplay()
{
    glutPostRedisplay();
}

void ENG_API Eng::Base::setDisplayCallback(void (*callback)())
{
    userDisplayCallback = callback;
    glutDisplayFunc(displayCallbackBridge);
}

void ENG_API Eng::Base::setReshapeCallback(void (*callback)(int, int))
{
    userReshapeCallback = callback;
    glutReshapeFunc(reshapeCallbackBridge);
}

void ENG_API Eng::Base::setKeyboardCallback(void (*callback)(unsigned char, int, int))
{
    userKeyboardCallback = callback;
    glutKeyboardFunc(keyboardCallbackBridge);
}

void ENG_API Eng::Base::setSpecialCallback(void (*callback)(int, int, int))
{
    userSpecialCallback = callback;
    glutSpecialFunc(specialCallbackBridge);
}

void ENG_API Eng::Base::setTimerCallback(unsigned int millis, void (*callback)(int), int value)
{
    userTimerCallback = callback;
    glutTimerFunc(millis, timerCallbackBridge, value);
}
/**
 * @file    engine.cpp
 * @brief   Graphics engine main file
 *
 * @author  Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */

#include "engine.h"
#include "node.h"
#include "ovoReader.h"

#include <iostream>
#include <source_location>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/freeglut.h>

#include <FreeImage.h>

// Callbacks
static void (*userDisplayCallback)() = nullptr;
static void (*userReshapeCallback)(int, int) = nullptr;
static void (*userKeyboardCallback)(unsigned char, int, int) = nullptr;
static void (*userSpecialCallback)(int, int, int) = nullptr;
static void (*userTimerCallback)(int) = nullptr;

/**
 * @brief Forwards the FreeGLUT display callback to the client.
 */
static void displayCallbackBridge()
{
    if (userDisplayCallback)
        userDisplayCallback();
}

/**
 * @brief Forwards the FreeGLUT keyboard callback to the client.
 * @param key Pressed key
 * @param x Mouse X coordinate
 * @param y Mouse Y coordinate
 */
static void keyboardCallbackBridge(unsigned char key, int x, int y)
{
    if (userKeyboardCallback)
        userKeyboardCallback(key, x, y);
}

/**
 * @brief Forwards the FreeGLUT special key callback to the client.
 * @param key Pressed special key
 * @param x Mouse X coordinate
 * @param y Mouse Y coordinate
 */
static void specialCallbackBridge(int key, int x, int y)
{
    if (userSpecialCallback)
        userSpecialCallback(key, x, y);
}

/**
 * @brief Forwards the FreeGLUT reshape callback to the client.
 * @param width New window width
 * @param height New window height
 */
static void reshapeCallbackBridge(int width, int height)
{
    if (userReshapeCallback)
        userReshapeCallback(width, height);
}

/**
 * @brief Forwards the FreeGLUT timer callback to the client.
 * @param value Timer value
 */
static void timerCallbackBridge(int value)
{
    if (userTimerCallback)
        userTimerCallback(value);
}

/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Stores the private internal state of the engine.
 */
struct Eng::Base::Reserved
{
    // Engine state
    bool initFlag;
    bool running;
    int windowId;

    /**
     * @brief Creates the reserved engine state.
     */
    Reserved() :
        initFlag{ false },
        running{ false },
        windowId{ 0 }
    {}
};

/**
 * @brief Creates the graphics engine.
 */
ENG_API Eng::Base::Base() :
    currentCamera{ nullptr },
    lastFpsTime{ std::chrono::steady_clock::now() },
    reserved(std::make_unique<Eng::Base::Reserved>())
{
#ifdef _DEBUG
    std::cout << "[ENGINE DEBUG] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}

/**
 * @brief Destroys the graphics engine.
 */
ENG_API Eng::Base::~Base()
{
#ifdef _DEBUG
    std::cout << "[ENGINE DEBUG] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}

/**
 * @brief Returns the unique singleton instance of the engine.
 * @return The singleton instance
 */
Eng::Base ENG_API& Eng::Base::getInstance()
{
    static Base instance;
    return instance;
}

/**
 * @brief Initializes the graphics engine and creates the rendering window.
 * @param windowTitle Title of the window
 * @param width Initial window width
 * @param height Initial window height
 * @return True if initialization succeeds, false if the engine is already initialized
 */
bool ENG_API Eng::Base::init(const char* windowTitle, int width, int height)
{
    if (reserved->initFlag)
        return false;

    // Initialize FreeImage
    FreeImage_Initialise();

    // Arguments required by FreeGLUT
    int argc = 1;
    char* argv[1] = { (char*)"engine" };

    // Initialize FreeGLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    // Create the rendering window
    reserved->windowId = glutCreateWindow(windowTitle);

    // Configure the initial OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);

    reserved->initFlag = true;
    reserved->running = true;

    std::cout << "[ENGINE] " << LIB_NAME << " initialized" << std::endl;

    return true;
}

/**
 * @brief Releases the internal resources of the graphics engine.
 * @return True if the engine was deinitialized, false if it was not initialized
 */
bool ENG_API Eng::Base::free()
{
    if (!reserved->initFlag)
    {
        std::cerr << "[ENGINE ERROR] Engine not initialized" << std::endl;
        return false;
    }

    FreeImage_DeInitialise();

    std::cout << "[ENGINE] " << LIB_NAME << " deinitialized" << std::endl;

    reserved->initFlag = false;
    reserved->running = false;

    return true;
}

/**
 * @brief Returns whether the engine is running.
 * @return True if the engine is running, false otherwise
 */
bool ENG_API Eng::Base::isRunning() const
{
    return reserved->running;
}

/**
 * @brief Clears the color and depth buffers of the rendering window.
 */
void ENG_API Eng::Base::clearWindow()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief Renders the FPS counter and swaps the front and back buffers.
 */
void ENG_API Eng::Base::swapBuffers()
{
    renderFPS();
    glutSwapBuffers();
}

/**
 * @brief Sets the background color of the rendering window.
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 */
void ENG_API Eng::Base::setBackgroundColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

/**
 * @brief Sets the OpenGL viewport.
 * @param x X coordinate of the viewport
 * @param y Y coordinate of the viewport
 * @param width Width of the viewport
 * @param height Height of the viewport
 */
void ENG_API Eng::Base::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

/**
 * @brief Replaces the current OpenGL matrix with the identity matrix.
 */
void ENG_API Eng::Base::loadIdentity()
{
    glLoadIdentity();
}

/**
 * @brief Starts the FreeGLUT main event loop.
 */
void ENG_API Eng::Base::mainLoop()
{
    glutMainLoop();
}

/**
 * @brief Requests a new rendering cycle.
 */
void ENG_API Eng::Base::postRedisplay()
{
    glutPostRedisplay();
}

/**
 * @brief Registers the client display callback.
 * @param callback Display callback function
 */
void ENG_API Eng::Base::setDisplayCallback(void (*callback)())
{
    userDisplayCallback = callback;
    glutDisplayFunc(displayCallbackBridge);
}

/**
 * @brief Registers the client reshape callback.
 * @param callback Reshape callback function
 */
void ENG_API Eng::Base::setReshapeCallback(void (*callback)(int, int))
{
    userReshapeCallback = callback;
    glutReshapeFunc(reshapeCallbackBridge);
}

/**
 * @brief Registers the client keyboard callback.
 * @param callback Keyboard callback function
 */
void ENG_API Eng::Base::setKeyboardCallback(void (*callback)(unsigned char, int, int))
{
    userKeyboardCallback = callback;
    glutKeyboardFunc(keyboardCallbackBridge);
}

/**
 * @brief Registers the client special key callback.
 * @param callback Special key callback function
 */
void ENG_API Eng::Base::setSpecialCallback(void (*callback)(int, int, int))
{
    userSpecialCallback = callback;
    glutSpecialFunc(specialCallbackBridge);
}

/**
 * @brief Registers the client timer callback.
 * @param millis Timer interval in milliseconds
 * @param callback Timer callback function
 * @param value Value passed to the callback
 */
void ENG_API Eng::Base::setTimerCallback(unsigned int millis, void (*callback)(int), int value)
{
    userTimerCallback = callback;
    glutTimerFunc(millis, timerCallbackBridge, value);
}

/**
 * @brief Adds a camera to the engine.
 * @param camera Camera to add
 */
void Eng::Base::addCamera(Camera* camera)
{
    if (camera == nullptr)
        return;

    cameras.push_back(camera);

    // Use the first added camera as the active camera
    if (currentCamera == nullptr)
        currentCamera = camera;
}

/**
 * @brief Sets the active camera.
 * @param camera Camera to activate
 */
void Eng::Base::setCamera(Camera* camera)
{
    currentCamera = camera;
}

/**
 * @brief Returns the active camera.
 * @return The active camera
 */
Eng::Camera* Eng::Base::getCamera() const
{
    return currentCamera;
}

/**
 * @brief Returns a camera at the specified index.
 * @param index Index of the camera
 * @return The camera, or nullptr if the index is invalid
 */
Eng::Camera* Eng::Base::getCameraAt(unsigned int index) const
{
    if (index >= cameras.size())
        return nullptr;

    return cameras[index];
}

/**
 * @brief Returns all cameras registered in the engine.
 * @return The camera list
 */
const std::vector<Eng::Camera*>& Eng::Base::getCameras() const
{
    return cameras;
}

/**
 * @brief Builds a render list from the specified scene graph.
 * @param root Root node of the scene graph
 * @return The generated render list
 */
Eng::List* Eng::Base::buildList(Node* root)
{
    List* list = new List("renderList");
    list->pass(root);
    return list;
}

/**
 * @brief Renders all elements contained in the render list.
 * @param list Render list to process
 */
void ENG_API Eng::Base::render(Eng::List* list)
{
    if (!list || !currentCamera)
        return;

    // Load the projection matrix of the active camera
    glm::mat4 projectionMatrix = currentCamera->getProjectionMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projectionMatrix));

    // Get the view matrix of the active camera
    glm::mat4 viewMatrix = currentCamera->getViewMatrix();

    // Get the maximum number of lights supported by OpenGL
    GLint maxLights = 0;
    glGetIntegerv(GL_MAX_LIGHTS, &maxLights);

    GLint lightIndex = 0;

    // Render each element in the render list
    for (const RenderElement& element : list->getElements())
    {
        if (element.node == nullptr)
            continue;

        // Configure light nodes
        if (auto* light = dynamic_cast<Light*>(element.node))
        {
            if (lightIndex >= maxLights)
                continue;

            glEnable(GL_LIGHTING);

            // Assign an OpenGL light identifier
            GLenum lightId = GL_LIGHT0 + lightIndex;
            glEnable(lightId);

            light->renderLight(element.worldMatrix, viewMatrix, lightId);

            ++lightIndex;
        }
        else
        {
            // Transform the object from local space to camera space
            glm::mat4 modelViewMatrix = viewMatrix * element.worldMatrix;

            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(modelViewMatrix));

            element.node->render();
        }
    }

    // Enable lighting only if at least one light was rendered
    if (lightIndex > 0)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
}

/**
 * @brief Loads a scene from an OVO file.
 * @param filename Path of the OVO file
 * @return The root node of the loaded scene
 */
Eng::Node* Eng::Base::load(const std::string& filename)
{
    OvoReader reader;
    return reader.load(filename);
}

/**
 * @brief Enables or disables wireframe rendering.
 * @param enabled True to enable wireframe mode, false to use filled polygons
 */
void Eng::Base::setWireframe(bool enabled)
{
    wireframe = enabled;

    glPolygonMode(
        GL_FRONT_AND_BACK,
        wireframe ? GL_LINE : GL_FILL
    );
}

/**
 * @brief Returns whether wireframe rendering is enabled.
 * @return True if wireframe mode is enabled, false otherwise
 */
bool Eng::Base::isWireframe() const
{
    return wireframe;
}

/**
 * @brief Enables or disables smooth shading.
 * @param enabled True for Gouraud shading, false for flat shading
 */
void Eng::Base::setSmoothShading(bool enabled)
{
    smoothShading = enabled;
    glShadeModel(enabled ? GL_SMOOTH : GL_FLAT);
}

/**
 * @brief Returns whether smooth shading is enabled.
 * @return True if smooth shading is enabled, false otherwise
 */
bool Eng::Base::isSmoothShading()
{
    return smoothShading;
}

/**
 * @brief Calculates the current FPS.
 */
void Eng::Base::calculateFPS()
{
    ++frameCount;

    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - lastFpsTime).count();

    // Update the FPS value once per second
    if (elapsed >= 1.0f)
    {
        fps = static_cast<float>(frameCount) / elapsed;
        frameCount = 0;
        lastFpsTime = now;
    }
}

/**
 * @brief Renders text in screen coordinates.
 * @param text Text to render
 * @param x Horizontal screen position
 * @param y Vertical screen position
 * @param r Red component of the text color
 * @param g Green component of the text color
 * @param b Blue component of the text color
 */
void Eng::Base::renderText(const std::string& text, int x, int y, float r, float g, float b)
{
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // Save the OpenGL states modified by the overlay
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    GLboolean textureEnabled = glIsEnabled(GL_TEXTURE_2D);
    GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);

    // Save the current rendering color
    GLfloat previousColor[4];
    glGetFloatv(GL_CURRENT_COLOR, previousColor);

    // Save the currently active matrix mode
    GLint previousMatrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &previousMatrixMode);

    // Overlay must not be affected by the 3D scene
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    // Create a 2D orthographic projection matching the window size
    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(width),
        0.0f,
        static_cast<float>(height),
        -1.0f,
        1.0f
    );

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));

    // Text does not require camera or world transformations
    glm::mat4 modelView(1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(modelView));

    // Configure text color and screen position
    glColor3f(r, g, b);
    glRasterPos2i(x, y);

    // Render each character using the FreeGLUT bitmap font
    for (char character : text)
    {
        glutBitmapCharacter(
            GLUT_BITMAP_8_BY_13,
            character
        );
    }

    // Restore the previous color
    glColor4fv(previousColor);

    // Restore lighting state
    if (lightingEnabled)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);

    // Restore texture state
    if (textureEnabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    // Restore depth testing state
    if (depthEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // Restore the active camera matrices
    if (currentCamera != nullptr)
    {
        glm::mat4 projectionMatrix = currentCamera->getProjectionMatrix();
        glm::mat4 viewMatrix = currentCamera->getViewMatrix();

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projectionMatrix));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(viewMatrix));
    }

    // Restore the previously active matrix mode
    glMatrixMode(previousMatrixMode);
}

/**
 * @brief Calculates and renders the current FPS counter.
 */
void Eng::Base::renderFPS()
{
    calculateFPS();

    int height = glutGet(GLUT_WINDOW_HEIGHT);
    std::string text = "FPS: " + std::to_string(static_cast<int>(fps));

    // Render the FPS counter in the upper-left corner
    renderText(text, 10, height - 20);
}
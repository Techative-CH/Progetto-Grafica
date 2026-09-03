/**
 * @file        main.cpp
 * @brief       Client application (that uses the graphics engine)
 *
 * @author      Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */

// Library header
#include "engine.h"

// GLM
#include <glm/gtc/matrix_transform.hpp>

// Standard libraries
#include <iostream>
#include <chrono>
#include <cmath>

// Hanoi
#include "hanoiGame.h"
#include "hanoiScene.h"


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

Eng::Node* root = nullptr;
Eng::List* renderList = nullptr;

// Cameras
Eng::Camera* camera = nullptr;
Eng::Camera* secondaryCamera = nullptr;

unsigned int currentCameraIndex = 0;

// Secondary camera orbit
glm::vec3 secondaryCameraTarget(-40.0f, 25.0f, 0.0f);

constexpr float SECONDARY_CAMERA_RADIUS = 137.3f;
constexpr float SECONDARY_CAMERA_HEIGHT = 85.0f;

constexpr float SECONDARY_CAMERA_START_ANGLE = 10.5f;
constexpr float SECONDARY_CAMERA_MIN_ANGLE = SECONDARY_CAMERA_START_ANGLE - 45.0f;
constexpr float SECONDARY_CAMERA_MAX_ANGLE = SECONDARY_CAMERA_START_ANGLE + 90.0f;
constexpr float SECONDARY_CAMERA_ANGLE_STEP = 1.0f;

float secondaryCameraAngle = SECONDARY_CAMERA_START_ANGLE;

// Viewport
int viewportWidth = 800;
int viewportHeight = 600;

// Hanoi
HanoiGame game;
HanoiScene hanoiScene;

int selectedRod = 0;
int sourceRod = -1;

// Frame time
std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();

// Sun animation
Eng::Node* sunNode = nullptr;
glm::mat4 sunOriginalMatrix;

float sunAngle = 0.0f;

constexpr float SUN_ROTATION_SPEED = 15.0f;

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////

void resetHanoi();
void undoHanoi();
void redoHanoi();

bool initSun();
void updateSun(float deltaTime);

void switchCamera();
void updateSecondaryCamera();
void moveSecondaryCamera(float angleDelta);

////////////////////////
// CALLBACK FUNCTIONS //
////////////////////////

void displayCallback()
{
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();

    lastFrameTime = currentTime;

    hanoiScene.update(deltaTime);
    updateSun(deltaTime);

    Eng::Base& eng = Eng::Base::getInstance();

    eng.clearWindow();
    eng.loadIdentity();

    if (root != nullptr && renderList != nullptr)
    {
        renderList->pass(root);
        eng.render(renderList);
    }

    hanoiScene.renderControls();

    eng.swapBuffers();
    eng.postRedisplay();
}

void reshapeCallback(int width, int height)
{
    if (height == 0)
        height = 1;

    viewportWidth = width;
    viewportHeight = height;

    Eng::Base& eng = Eng::Base::getInstance();

    eng.setViewport(0, 0, width, height);

    // Set aspect ration for each camera
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    for (Eng::Camera* camera : eng.getCameras())
    {
        camera->setPerspective(75.0f, aspect, 0.1f, 1000.0f);
    }
}

void keyboardCallback(unsigned char key, int mouseX, int mouseY)
{
    Eng::Base& eng = Eng::Base::getInstance();

    switch (key)
    {
    case 27:
        exit(0);
        break;

    // Pick / drop disk
    case ' ':
    {
        if (hanoiScene.isAnimating())
            break;

        // Pick
        if (sourceRod == -1)
        {
            int disk = game.getTopDisk(selectedRod);
            if (disk != -1 && hanoiScene.selectDisk(disk))
                sourceRod = selectedRod;
        }
        else
        {
            // Drop
            int disk = game.getTopDisk(sourceRod);
            if (game.moveDisk(sourceRod, selectedRod))
            {
                // Move disk to target at the correct height
                int destinationLevel = game.getRodSize(selectedRod) - 1;
                hanoiScene.moveDisk(disk, selectedRod, destinationLevel);

                if (game.isSolved())
                {
                    std::cout << "Tower of Hanoi solved!" << std::endl;
                }
            }
            else
            {
                hanoiScene.cancelDiskSelection();
            }

            sourceRod = -1;
        }

        break;
    }
    
    // Reset
    case 'r':
    case 'R':
        resetHanoi();
        break;

    // Undo
    case 'z':
    case 'Z':
        undoHanoi();
        break;
    
    // Redo
    case 'y':
    case 'Y':
        redoHanoi();
        break;

    // Switch camera
    case 'c':
    case 'C':
        switchCamera();
        break;

    // Orbit secondary camera
    case 'a':
    case 'A':
        if (eng.getCamera() == secondaryCamera)
            moveSecondaryCamera(-SECONDARY_CAMERA_ANGLE_STEP);
        break;

    case 'd':
    case 'D':
        if (eng.getCamera() == secondaryCamera)
            moveSecondaryCamera(SECONDARY_CAMERA_ANGLE_STEP);
        break;
    
    // Wireframe mode
    case 'w':
    case 'W':
        eng.setWireframe(!eng.isWireframe());
        break;

    // Smooth shading
    case 's':
    case 'S':
        eng.setSmoothShading(!eng.isSmoothShading());
        break;

    // Help
    case 'h':
    case 'H':
        hanoiScene.toggleControls();
        break;
    }

    eng.postRedisplay();
}

void specialCallback(int key, int mouseX, int mouseY)
{
    if (hanoiScene.isAnimating())
        return;

    switch (key)
    {
    case 100: // Left arrow
        selectedRod--;
        if (selectedRod < 0)
            selectedRod = HanoiGame::NUM_RODS - 1;
        break;

    case 102: // Right arrow
        selectedRod++;
        if (selectedRod >= HanoiGame::NUM_RODS)
            selectedRod = 0;
        break;

    default:
        return;
    }

    hanoiScene.updateRodSelection(selectedRod);

    Eng::Base::getInstance().postRedisplay();
}

//////////////////////////
// HANOI GAME FUNCTIONS //
//////////////////////////

void resetHanoi()
{
    game.reset();
    hanoiScene.reset();

    selectedRod = 0;
    sourceRod = -1;

    hanoiScene.updateRodSelection(selectedRod);
}

void undoHanoi()
{
    if (hanoiScene.isAnimating())
        return;

    if (sourceRod != -1)
        return;

    Move move;

    if (!game.undo(move))
        return;

    // Move disk to target rod at correct height
    int destinationLevel = game.getRodSize(move.from) - 1;
    hanoiScene.moveDisk(move.disk, move.from, destinationLevel);

    selectedRod = move.from;
    hanoiScene.updateRodSelection(selectedRod);
}

void redoHanoi()
{
    if (hanoiScene.isAnimating())
        return;

    if (sourceRod != -1)
        return;

    Move move;

    if (!game.redo(move))
        return;

    // Move disk to target rod at correct height
    int destinationLevel = game.getRodSize(move.to) - 1;
    hanoiScene.moveDisk(move.disk, move.to, destinationLevel);

    selectedRod = move.to;
    hanoiScene.updateRodSelection(selectedRod);
}

///////////////////
// SUN FUNCTIONS //
///////////////////

bool initSun()
{
    sunNode = root->findByName("Sun");

    if (sunNode == nullptr)
    {
        std::cerr << "Unable to find Sun" << std::endl;
        return false;
    }

    sunOriginalMatrix = sunNode->getLocalMatrix();

    return true;
}

void updateSun(float deltaTime)
{
    if (sunNode == nullptr)
        return;

    sunAngle += SUN_ROTATION_SPEED * deltaTime;

    if (sunAngle >= 360.0f)
        sunAngle -= 360.0f;

    glm::mat4 rotation = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(sunAngle),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    sunNode->setLocalMatrix(sunOriginalMatrix * rotation);
}

//////////////////////
// CAMERA FUNCTIONS //
//////////////////////

void switchCamera()
{
    Eng::Base& eng = Eng::Base::getInstance();
    const auto& cameras = eng.getCameras();

    if (cameras.empty())
        return;

    currentCameraIndex = (currentCameraIndex + 1) % cameras.size();
    eng.setCamera(cameras[currentCameraIndex]);
}

void updateSecondaryCamera()
{
    if (secondaryCamera == nullptr)
        return;

    float angle = glm::radians(secondaryCameraAngle);
    float x = secondaryCameraTarget.x + SECONDARY_CAMERA_RADIUS * std::sin(angle);
    float z = secondaryCameraTarget.z + SECONDARY_CAMERA_RADIUS * std::cos(angle);

    glm::vec3 position(x, SECONDARY_CAMERA_HEIGHT, z);

    glm::mat4 view = glm::lookAt(
        position, 
        secondaryCameraTarget,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    secondaryCamera->setLocalMatrix(
        glm::inverse(view)
    );
}

void moveSecondaryCamera(float angleDelta)
{
    secondaryCameraAngle += angleDelta;

    if (secondaryCameraAngle < SECONDARY_CAMERA_MIN_ANGLE)
        secondaryCameraAngle = SECONDARY_CAMERA_MIN_ANGLE;

    if (secondaryCameraAngle > SECONDARY_CAMERA_MAX_ANGLE)
        secondaryCameraAngle = SECONDARY_CAMERA_MAX_ANGLE;

    updateSecondaryCamera();
}

//////////
// MAIN //
//////////

/**
 * Application entry point.
 * @param argc number of command-line arguments passed
 * @param argv array containing up to argc passed arguments
 * @return error code (0 on success, error code otherwise)
 */
int main(int argc, char* argv[])
{
    std::cout << "Client - Tower of Hanoi, S. Banfi (C) SUPSI" << std::endl << std::endl;

    // Initialize engine
    Eng::Base& eng = Eng::Base::getInstance();

    eng.init("Tower of Hanoi", 800, 600);
    eng.setBackgroundColor(0.0f, 0.0f, 0.0f);

    // Load OVO scene
    root = eng.load("assets/hanoi/hanoi.ovo");

    if (root == nullptr)
    {
        std::cerr << "OVO loading failed" << std::endl;
        eng.free();
        return -1;
    }

    std::cout << "OVO loaded successfully" << std::endl;

    // Sun
    if (!initSun())
        std::cerr << "Unable to initialize Sun" << std::endl;
    
    // Initialize Hanoi game
    game.init();

    if (!hanoiScene.init(root))
    {
        std::cerr << "Unable to initialize Hanoi scene" << std::endl;

        delete root;
        root = nullptr;

        eng.free();

        return -1;
    }

    std::cout << "Hanoi game initialized successfully" << std::endl;

    hanoiScene.updateRodSelection(selectedRod);

    // Build render list
    renderList = eng.buildList(root);

    if (renderList == nullptr)
    {
        std::cerr << "Unable to create render list" << std::endl;

        delete root;
        root = nullptr;

        eng.free();

        return -1;
    }

    // Main camera
    camera = new Eng::Camera("MainCamera");

    glm::vec3 cameraPosition(-36.0f, 39.0f, 60.0f);
    glm::vec3 cameraTarget(-40.0f, 18.0f, 12.0f);

    glm::mat4 view = glm::lookAt(
        cameraPosition,
        cameraTarget,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    camera->setLocalMatrix(
        glm::inverse(view)
    );

    camera->setPerspective(75.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
    eng.addCamera(camera);

    // Secondary camera
    secondaryCamera = new Eng::Camera("SecondaryCamera");
    secondaryCamera->setPerspective(75.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
    updateSecondaryCamera(); // init position
    eng.addCamera(secondaryCamera);

    // Bind callbacks to engine
    eng.setDisplayCallback(displayCallback);
    eng.setReshapeCallback(reshapeCallback);
    eng.setKeyboardCallback(keyboardCallback);
    eng.setSpecialCallback(specialCallback);

    // Start main loop
    std::cout << "Engine started correctly" << std::endl;
    eng.mainLoop();

    // Cleanup
    delete renderList;
    renderList = nullptr;

    delete root;
    root = nullptr;

    eng.free();

    std::cout << "\n[application terminated]" << std::endl;

    return 0;
}
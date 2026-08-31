/**
 * @file        main.cpp
 * @brief       Client application (that uses the graphics engine)
 *
 * @author      Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */


 //////////////
 // #INCLUDE //
 //////////////

 // Library header:
#include "engine.h"
#include <glm/gtc/matrix_transform.hpp>

// C/C++:
#include <iostream>
#include <chrono>
#include <cmath>

// Hanoi:
#include "hanoiGame.h"
#include "hanoiScene.h"


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

Eng::Node* root = nullptr;
Eng::List* renderList = nullptr;


// Cameras:
Eng::Camera* camera = nullptr;
Eng::Camera* secondaryCamera = nullptr;

unsigned int currentCameraIndex = 0;


// Secondary camera orbit:
glm::vec3 secondaryCameraTarget(
    -40.0f,
    25.0f,
    0.0f
);

constexpr float SECONDARY_CAMERA_RADIUS = 137.3f;
constexpr float SECONDARY_CAMERA_HEIGHT = 85.0f;

constexpr float SECONDARY_CAMERA_START_ANGLE = 10.5f;

constexpr float SECONDARY_CAMERA_MIN_ANGLE =
SECONDARY_CAMERA_START_ANGLE - 45.0f;

constexpr float SECONDARY_CAMERA_MAX_ANGLE =
SECONDARY_CAMERA_START_ANGLE + 90.0f;

constexpr float SECONDARY_CAMERA_ANGLE_STEP = 1.0f;

float secondaryCameraAngle =
SECONDARY_CAMERA_START_ANGLE;


// Hanoi:
HanoiGame game;
HanoiScene hanoiScene;

int selectedRod = 0;
int sourceRod = -1;


// Frame time:
std::chrono::steady_clock::time_point lastFrameTime =
std::chrono::steady_clock::now();


// Sun animation:
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

void printNodePositions(
    Eng::Node* node,
    int depth = 0
);

bool initSun();

void updateSun(
    float deltaTime
);

void switchCamera();

void updateSecondaryCamera();

void moveSecondaryCamera(
    float angleDelta
);


////////////////////////
// CALLBACK FUNCTIONS //
////////////////////////

void displayCallback()
{
    auto currentTime =
        std::chrono::steady_clock::now();

    float deltaTime =
        std::chrono::duration<float>(
            currentTime - lastFrameTime
        ).count();

    lastFrameTime = currentTime;

    hanoiScene.update(deltaTime);
    updateSun(deltaTime);

    Eng::Base& eng =
        Eng::Base::getInstance();

    eng.clearWindow();
    eng.loadIdentity();

    if (root != nullptr &&
        renderList != nullptr)
    {
        renderList->pass(root);
        eng.render(renderList);
    }

    eng.swapBuffers();
    eng.postRedisplay();
}


void reshapeCallback(
    int width,
    int height
)
{
    Eng::Base& eng =
        Eng::Base::getInstance();

    eng.setViewport(
        0,
        0,
        width,
        height
    );

    float aspect =
        static_cast<float>(width) /
        static_cast<float>(height);

    for (Eng::Camera* camera :
        eng.getCameras())
    {
        camera->setPerspective(
            75.0f,
            aspect,
            0.1f,
            1000.0f
        );
    }
}


void keyboardCallback(
    unsigned char key,
    int mouseX,
    int mouseY
)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;


        //////////
        // MOVE //
        //////////

    case ' ':
    {
        if (hanoiScene.isAnimating())
            break;

        if (sourceRod == -1)
        {
            // Pick up:
            int disk =
                game.getTopDisk(
                    selectedRod
                );

            if (disk != -1)
            {
                if (hanoiScene.selectDisk(disk))
                {
                    sourceRod =
                        selectedRod;

                    std::cout
                        << "Selected Disk_"
                        << disk
                        << " from rod "
                        << sourceRod
                        << std::endl;
                }
            }
        }
        else
        {
            // Drop:
            int disk =
                game.getTopDisk(
                    sourceRod
                );

            if (game.moveDisk(
                sourceRod,
                selectedRod
            ))
            {
                int destinationLevel =
                    game.getRodSize(
                        selectedRod
                    ) - 1;

                hanoiScene.moveDisk(
                    disk,
                    selectedRod,
                    destinationLevel
                );

                std::cout
                    << "Moved Disk_"
                    << disk
                    << " from rod "
                    << sourceRod
                    << " to rod "
                    << selectedRod
                    << std::endl;

                game.printState();

                if (game.isSolved())
                {
                    std::cout
                        << "Tower of Hanoi solved!"
                        << std::endl;
                }
            }
            else
            {
                hanoiScene
                    .cancelDiskSelection();

                std::cout
                    << "Invalid move"
                    << std::endl;
            }

            sourceRod = -1;
        }

        break;
    }


    ///////////
    // RESET //
    ///////////

    case 'r':
    case 'R':
        resetHanoi();
        break;


        //////////
        // UNDO //
        //////////

    case 'z':
    case 'Z':
        undoHanoi();
        break;


        //////////
        // REDO //
        //////////

    case 'y':
    case 'Y':
        redoHanoi();
        break;


        ///////////////////
        // SWITCH CAMERA //
        ///////////////////

    case 'c':
    case 'C':
        switchCamera();
        break;


        /////////////////////////////
        // SECONDARY CAMERA ORBIT //
        /////////////////////////////

    case 'a':
    case 'A':
        if (Eng::Base::getInstance().getCamera() ==
            secondaryCamera)
        {
            moveSecondaryCamera(
                -SECONDARY_CAMERA_ANGLE_STEP
            );
        }

        break;

    case 'd':
    case 'D':
        if (Eng::Base::getInstance().getCamera() ==
            secondaryCamera)
        {
            moveSecondaryCamera(
                SECONDARY_CAMERA_ANGLE_STEP
            );
        }

        break;
    }

    Eng::Base::getInstance()
        .postRedisplay();
}


void specialCallback(
    int key,
    int mouseX,
    int mouseY
)
{
    if (hanoiScene.isAnimating())
        return;

    switch (key)
    {
    case 100: // Left arrow

        selectedRod--;

        if (selectedRod < 0)
        {
            selectedRod =
                HanoiGame::NUM_RODS - 1;
        }

        break;


    case 102: // Right arrow

        selectedRod++;

        if (selectedRod >=
            HanoiGame::NUM_RODS)
        {
            selectedRod = 0;
        }

        break;


    default:
        return;
    }

    hanoiScene.updateRodSelection(
        selectedRod
    );

    std::cout
        << "Selected rod: "
        << selectedRod
        << std::endl;

    Eng::Base::getInstance()
        .postRedisplay();
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

    hanoiScene.updateRodSelection(
        selectedRod
    );

    std::cout
        << "Game reset"
        << std::endl;

    game.printState();
}


void undoHanoi()
{
    if (hanoiScene.isAnimating())
        return;

    if (sourceRod != -1)
        return;

    Move move;

    if (!game.undo(move))
    {
        std::cout
            << "Nothing to undo"
            << std::endl;

        return;
    }

    int destinationLevel =
        game.getRodSize(
            move.from
        ) - 1;

    hanoiScene.moveDisk(
        move.disk,
        move.from,
        destinationLevel
    );

    selectedRod =
        move.from;

    hanoiScene.updateRodSelection(
        selectedRod
    );

    std::cout
        << "Undo: Disk_"
        << move.disk
        << " from rod "
        << move.to
        << " to rod "
        << move.from
        << std::endl;

    game.printState();
}


void redoHanoi()
{
    if (hanoiScene.isAnimating())
        return;

    if (sourceRod != -1)
        return;

    Move move;

    if (!game.redo(move))
    {
        std::cout
            << "Nothing to redo"
            << std::endl;

        return;
    }

    int destinationLevel =
        game.getRodSize(
            move.to
        ) - 1;

    hanoiScene.moveDisk(
        move.disk,
        move.to,
        destinationLevel
    );

    selectedRod =
        move.to;

    hanoiScene.updateRodSelection(
        selectedRod
    );

    std::cout
        << "Redo: Disk_"
        << move.disk
        << " from rod "
        << move.from
        << " to rod "
        << move.to
        << std::endl;

    game.printState();
}


///////////////////
// SUN FUNCTIONS //
///////////////////

bool initSun()
{
    sunNode =
        root->findByName(
            "Sun"
        );

    if (sunNode == nullptr)
    {
        std::cerr
            << "Unable to find Sun"
            << std::endl;

        return false;
    }

    sunOriginalMatrix =
        sunNode->getLocalMatrix();

    return true;
}


void updateSun(
    float deltaTime
)
{
    if (sunNode == nullptr)
        return;

    sunAngle +=
        SUN_ROTATION_SPEED *
        deltaTime;

    if (sunAngle >= 360.0f)
    {
        sunAngle -= 360.0f;
    }

    glm::mat4 rotation =
        glm::rotate(
            glm::mat4(1.0f),
            glm::radians(sunAngle),
            glm::vec3(
                1.0f,
                0.0f,
                0.0f
            )
        );

    sunNode->setLocalMatrix(
        sunOriginalMatrix *
        rotation
    );
}


//////////////////////
// CAMERA FUNCTIONS //
//////////////////////

void switchCamera()
{
    Eng::Base& eng =
        Eng::Base::getInstance();

    const auto& cameras =
        eng.getCameras();

    if (cameras.empty())
        return;

    currentCameraIndex =
        (currentCameraIndex + 1) %
        cameras.size();

    eng.setCamera(
        cameras[currentCameraIndex]
    );

    std::cout
        << "Camera: "
        << cameras[currentCameraIndex]
        ->getName()
        << std::endl;
}


void updateSecondaryCamera()
{
    if (secondaryCamera == nullptr)
        return;

    float angle =
        glm::radians(
            secondaryCameraAngle
        );

    glm::vec3 position(
        secondaryCameraTarget.x +
        SECONDARY_CAMERA_RADIUS *
        std::sin(angle),

        SECONDARY_CAMERA_HEIGHT,

        secondaryCameraTarget.z +
        SECONDARY_CAMERA_RADIUS *
        std::cos(angle)
    );

    glm::mat4 view =
        glm::lookAt(
            position,
            secondaryCameraTarget,
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );

    secondaryCamera->setLocalMatrix(
        glm::inverse(view)
    );
}


void moveSecondaryCamera(
    float angleDelta
)
{
    secondaryCameraAngle +=
        angleDelta;

    if (secondaryCameraAngle <
        SECONDARY_CAMERA_MIN_ANGLE)
    {
        secondaryCameraAngle =
            SECONDARY_CAMERA_MIN_ANGLE;
    }

    if (secondaryCameraAngle >
        SECONDARY_CAMERA_MAX_ANGLE)
    {
        secondaryCameraAngle =
            SECONDARY_CAMERA_MAX_ANGLE;
    }

    updateSecondaryCamera();

    std::cout
        << "Secondary camera angle: "
        << secondaryCameraAngle
        << std::endl;
}


/////////////////////
// DEBUG FUNCTIONS //
/////////////////////

void printNodePositions(
    Eng::Node* node,
    int depth
)
{
    if (!node)
        return;

    glm::mat4 world =
        node->getWorldMatrix();

    glm::vec3 position(
        world[3][0],
        world[3][1],
        world[3][2]
    );

    std::cout
        << std::string(
            depth * 2,
            ' '
        )
        << node->getName()
        << " -> "
        << position.x
        << ", "
        << position.y
        << ", "
        << position.z
        << std::endl;

    for (Eng::Node* child :
        node->getChildren())
    {
        printNodePositions(
            child,
            depth + 1
        );
    }
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
int main(
    int argc,
    char* argv[]
)
{
    std::cout
        << "Client - Tower of Hanoi, S. Banfi (C) SUPSI"
        << std::endl
        << std::endl;

    Eng::Base& eng =
        Eng::Base::getInstance();


    ///////////////////////////
    // ENGINE INITIALIZATION //
    ///////////////////////////

    eng.init(
        "Tower of Hanoi",
        800,
        600
    );

    eng.setBackgroundColor(
        0.0f,
        0.0f,
        0.0f
    );


    ////////////////////
    // LOAD OVO SCENE //
    ////////////////////

    root =
        eng.load(
            "assets/hanoi/hanoi.ovo"
        );

    if (root == nullptr)
    {
        std::cerr
            << "OVO loading failed"
            << std::endl;

        eng.free();

        return -1;
    }

    printNodePositions(root);

    std::cout
        << "OVO loaded successfully"
        << std::endl;


    /////////
    // SUN //
    /////////

    if (!initSun())
    {
        std::cerr
            << "Unable to initialize Sun"
            << std::endl;
    }


    /////////////////////
    // INIT HANOI GAME //
    /////////////////////

    game.init();

    if (!hanoiScene.init(root))
    {
        std::cerr
            << "Unable to initialize Hanoi scene"
            << std::endl;

        delete root;
        root = nullptr;

        eng.free();

        return -1;
    }

    hanoiScene.updateRodSelection(
        selectedRod
    );

    game.printState();


    ///////////////////////
    // BUILD RENDER LIST //
    ///////////////////////

    renderList =
        eng.buildList(root);

    if (renderList == nullptr)
    {
        std::cerr
            << "Unable to create render list"
            << std::endl;

        delete root;
        root = nullptr;

        eng.free();

        return -1;
    }

    std::cout
        << "Render elements: "
        << renderList
        ->getElements()
        .size()
        << std::endl;


    //////////////////
    // MAIN CAMERA //
    //////////////////

    camera =
        new Eng::Camera(
            "MainCamera"
        );

    glm::vec3 cameraPosition(
        -36.0f,
        39.0f,
        60.0f
    );

    glm::vec3 cameraTarget(
        -40.0f,
        18.0f,
        12.0f
    );

    glm::mat4 view =
        glm::lookAt(
            cameraPosition,
            cameraTarget,
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );

    camera->setLocalMatrix(
        glm::inverse(view)
    );

    camera->setPerspective(
        75.0f,
        800.0f / 600.0f,
        0.1f,
        1000.0f
    );

    eng.addCamera(
        camera
    );


    //////////////////////
    // SECONDARY CAMERA //
    //////////////////////

    secondaryCamera =
        new Eng::Camera(
            "SecondaryCamera"
        );

    secondaryCamera->setPerspective(
        75.0f,
        800.0f / 600.0f,
        0.1f,
        1000.0f
    );

    // Calculate its initial position
    // using the orbital parameters:
    updateSecondaryCamera();

    eng.addCamera(
        secondaryCamera
    );


    ///////////////
    // CALLBACKS //
    ///////////////

    eng.setDisplayCallback(
        displayCallback
    );

    eng.setReshapeCallback(
        reshapeCallback
    );

    eng.setKeyboardCallback(
        keyboardCallback
    );

    eng.setSpecialCallback(
        specialCallback
    );


    /////////////////////
    // START MAIN LOOP //
    /////////////////////

    std::cout
        << "Engine started correctly"
        << std::endl;

    eng.mainLoop();


    /////////////
    // CLEANUP //
    /////////////

    delete renderList;
    renderList = nullptr;

    delete root;
    root = nullptr;

    eng.free();

    std::cout
        << "\n[application terminated]"
        << std::endl;

    return 0;
}
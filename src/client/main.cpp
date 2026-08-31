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
#include <vector>
#include <string>
#include <chrono>

// Hanoi Game
#include "hanoiGame.h"


/////////////////////////
// ANIMATION STRUCTURES //
/////////////////////////

enum class AnimationPhase
{
    NONE,
    LIFT,
    HORIZONTAL,
    DROP
};

struct DiskAnimation
{
    bool active = false;

    Eng::Node* disk = nullptr;

    glm::vec3 startPosition;
    glm::vec3 targetPosition;

    float liftHeight = 0.0f;

    AnimationPhase phase = AnimationPhase::NONE;
};


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

Eng::Node* root = nullptr;
Eng::List* renderList = nullptr;
Eng::Camera* camera = nullptr;

HanoiGame game;

std::vector<Eng::Node*> diskNodes;
std::vector<Eng::Node*> rodNodes;
std::vector<glm::mat4> rodOriginalMatrices;
std::vector<glm::mat4> diskOriginalMatrices;

const std::string rodNames[HanoiGame::NUM_RODS] =
{
    "Pole_Left",
    "Pole_Center",
    "Pole_Right"
};

float diskSpacing = 0.0f;
float baseDiskHeight = 0.0f;

int selectedRod = 0;
int sourceRod = -1;

Eng::Node* selectedDiskNode = nullptr;
glm::vec3 selectedDiskOriginalPosition;

DiskAnimation animation;

std::chrono::steady_clock::time_point lastFrameTime =
std::chrono::steady_clock::now();

constexpr float DISK_SPEED = 150.0f;
constexpr float LIFT_MARGIN = 15.0f;
constexpr float SELECTION_HEIGHT = 4.0f;
constexpr float ROD_SELECTION_HEIGHT = 4.0f;


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////

bool initHanoiNodes();

void resetHanoi();

void undoHanoi();

void redoHanoi();

void moveDiskNode(
    int disk,
    int destinationRod
);

void updateAnimation(
    float deltaTime
);

void updateRodSelection();

void printNodePositions(
    Eng::Node* node,
    int depth = 0
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

    updateAnimation(deltaTime);

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

    // Request next frame:
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

    if (camera != nullptr)
    {
        camera->setPerspective(
            75.0f,
            static_cast<float>(width) /
            static_cast<float>(height),
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

    case ' ':
        // No new move while animation is running:
        if (animation.active)
            break;

        if (sourceRod == -1)
        {
            //////////////////
            // PICK UP DISK //
            //////////////////

            int disk =
                game.getTopDisk(selectedRod);

            if (disk != -1)
            {
                sourceRod = selectedRod;

                selectedDiskNode =
                    diskNodes[disk - 1];

                glm::mat4 matrix =
                    selectedDiskNode
                    ->getLocalMatrix();

                selectedDiskOriginalPosition =
                    glm::vec3(matrix[3]);

                // Slightly lift selected disk:
                matrix[3][1] +=
                    SELECTION_HEIGHT;

                selectedDiskNode
                    ->setLocalMatrix(matrix);

                std::cout
                    << "Selected Disk_"
                    << disk
                    << " from rod "
                    << sourceRod
                    << std::endl;
            }
        }
        else
        {
            ///////////////
            // DROP DISK //
            ///////////////

            int disk =
                game.getTopDisk(sourceRod);

            if (game.moveDisk(
                sourceRod,
                selectedRod
            ))
            {
                moveDiskNode(
                    disk,
                    selectedRod
                );

                selectedDiskNode =
                    nullptr;

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
                //////////////////
                // INVALID MOVE //
                //////////////////

                if (selectedDiskNode != nullptr)
                {
                    glm::mat4 matrix =
                        selectedDiskNode
                        ->getLocalMatrix();

                    matrix[3][0] =
                        selectedDiskOriginalPosition.x;

                    matrix[3][1] =
                        selectedDiskOriginalPosition.y;

                    matrix[3][2] =
                        selectedDiskOriginalPosition.z;

                    selectedDiskNode
                        ->setLocalMatrix(matrix);

                    selectedDiskNode =
                        nullptr;
                }

                std::cout
                    << "Invalid move"
                    << std::endl;
            }

            sourceRod = -1;
        }

        break;

    case 'r':
    case 'R':
        resetHanoi();
        break;

    case 'z':
    case 'Z':
        undoHanoi();
        break;

    case 'y':
    case 'Y':
        redoHanoi();
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

    updateRodSelection();

    std::cout
        << "Selected rod: "
        << selectedRod
        << std::endl;

    Eng::Base::getInstance()
        .postRedisplay();
}


///////////////////////////
// HANOI SCENE FUNCTIONS //
///////////////////////////

bool initHanoiNodes()
{
    diskNodes.clear();
    rodNodes.clear();
    rodOriginalMatrices.clear();
    diskOriginalMatrices.clear();

    //////////
    // RODS //
    //////////

    for (
        int i = 0;
        i < HanoiGame::NUM_RODS;
        i++
        )
    {
        Eng::Node* rod =
            root->findByName(
                rodNames[i]
            );

        if (rod == nullptr)
        {
            std::cerr
                << "Unable to find rod: "
                << rodNames[i]
                << std::endl;

            return false;
        }

        rodNodes.push_back(rod);

        rodOriginalMatrices.push_back(
            rod->getLocalMatrix()
        );
    }


    ///////////
    // DISKS //
    ///////////

    for (
        int i = 1;
        i <= HanoiGame::NUM_DISKS;
        i++
        )
    {
        std::string diskName =
            "Disk_" +
            std::to_string(i);

        Eng::Node* disk =
            root->findByName(
                diskName
            );

        if (disk == nullptr)
        {
            std::cerr
                << "Unable to find disk: "
                << diskName
                << std::endl;

            return false;
        }

        diskNodes.push_back(disk);

        diskOriginalMatrices.push_back(
            disk->getLocalMatrix()
        );
    }


    //////////////////////////
    // DISK HEIGHT / SPACING //
    //////////////////////////

    if (diskNodes.size() < 2)
    {
        std::cerr
            << "At least two disks are required"
            << std::endl;

        return false;
    }

    glm::vec3 bottomPosition =
        glm::vec3(
            diskNodes[
                HanoiGame::NUM_DISKS - 1
            ]
            ->getLocalMatrix()[3]
        );

    glm::vec3 nextPosition =
        glm::vec3(
            diskNodes[
                HanoiGame::NUM_DISKS - 2
            ]
            ->getLocalMatrix()[3]
        );

    baseDiskHeight =
        bottomPosition.y;

    diskSpacing =
        nextPosition.y -
        bottomPosition.y;

    std::cout
        << "Base disk height: "
        << baseDiskHeight
        << ", spacing: "
        << diskSpacing
        << std::endl;

    return true;
}

void resetHanoi()
{
    // Cancel any running animation:
    animation.active = false;
    animation.disk = nullptr;
    animation.phase = AnimationPhase::NONE;

    // Cancel current disk selection:
    selectedDiskNode = nullptr;
    sourceRod = -1;

    // Reset logical game state:
    game.reset();

    // Restore every disk to its original matrix:
    for (int i = 0; i < HanoiGame::NUM_DISKS; i++)
    {
        diskNodes[i]->setLocalMatrix(
            diskOriginalMatrices[i]
        );
    }

    // Select the first rod:
    selectedRod = 0;

    updateRodSelection();

    std::cout
        << "Game reset"
        << std::endl;

    game.printState();
}

void undoHanoi()
{
    if (animation.active)
        return;

    // Don't undo while a disk is manually selected:
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

    // Undo means moving the disk back to its original rod.
    moveDiskNode(
        move.disk,
        move.from
    );

    // Follow the destination of the undo with the rod selector:
    selectedRod = move.from;
    updateRodSelection();

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
    if (animation.active)
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

    moveDiskNode(
        move.disk,
        move.to
    );

    selectedRod = move.to;
    updateRodSelection();

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

void updateRodSelection()
{
    for (
        int i = 0;
        i < HanoiGame::NUM_RODS;
        i++
        )
    {
        glm::mat4 matrix =
            rodOriginalMatrices[i];

        if (i == selectedRod)
        {
            matrix[3][1] +=
                ROD_SELECTION_HEIGHT;
        }

        rodNodes[i]
            ->setLocalMatrix(matrix);
    }
}


void moveDiskNode(
    int disk,
    int destinationRod
)
{
    if (
        disk < 1 ||
        disk > HanoiGame::NUM_DISKS
        )
    {
        return;
    }

    if (
        destinationRod < 0 ||
        destinationRod >=
        HanoiGame::NUM_RODS
        )
    {
        return;
    }

    Eng::Node* diskNode =
        diskNodes[disk - 1];

    Eng::Node* rodNode =
        rodNodes[destinationRod];

    if (
        diskNode == nullptr ||
        rodNode == nullptr
        )
    {
        return;
    }

    glm::mat4 diskMatrix =
        diskNode->getLocalMatrix();

    glm::mat4 rodMatrix =
        rodOriginalMatrices[
            destinationRod
        ];

    glm::vec3 startPosition =
        glm::vec3(
            diskMatrix[3]
        );

    // Important:
    // use original rod position, not the
    // visually lifted selected rod.
    glm::vec3 rodPosition =
        glm::vec3(
            rodMatrix[3]
        );

    int level =
        game.getRodSize(
            destinationRod
        ) - 1;

    glm::vec3 targetPosition(
        rodPosition.x,
        baseDiskHeight +
        diskSpacing * level,
        rodPosition.z
    );

    float liftHeight =
        baseDiskHeight +
        diskSpacing *
        HanoiGame::NUM_DISKS +
        LIFT_MARGIN;

    animation.active = true;
    animation.disk = diskNode;

    animation.startPosition =
        startPosition;

    animation.targetPosition =
        targetPosition;

    animation.liftHeight =
        liftHeight;

    animation.phase =
        AnimationPhase::LIFT;
}


void updateAnimation(
    float deltaTime
)
{
    if (
        !animation.active ||
        animation.disk == nullptr
        )
    {
        return;
    }

    glm::mat4 matrix =
        animation.disk
        ->getLocalMatrix();

    glm::vec3 position =
        glm::vec3(matrix[3]);

    float movement =
        DISK_SPEED *
        deltaTime;

    bool completed =
        false;

    switch (animation.phase)
    {
    case AnimationPhase::LIFT:
    {
        position.y += movement;

        if (
            position.y >=
            animation.liftHeight
            )
        {
            position.y =
                animation.liftHeight;

            animation.phase =
                AnimationPhase::HORIZONTAL;
        }

        break;
    }

    case AnimationPhase::HORIZONTAL:
    {
        glm::vec3 horizontalTarget(
            animation.targetPosition.x,
            animation.liftHeight,
            animation.targetPosition.z
        );

        glm::vec3 difference =
            horizontalTarget -
            position;

        float distance =
            glm::length(
                difference
            );

        if (distance <= movement)
        {
            position =
                horizontalTarget;

            animation.phase =
                AnimationPhase::DROP;
        }
        else
        {
            position +=
                glm::normalize(
                    difference
                ) * movement;
        }

        break;
    }

    case AnimationPhase::DROP:
    {
        position.y -= movement;

        if (
            position.y <=
            animation.targetPosition.y
            )
        {
            position =
                animation.targetPosition;

            completed = true;
        }

        break;
    }

    case AnimationPhase::NONE:
        return;
    }

    matrix[3][0] =
        position.x;

    matrix[3][1] =
        position.y;

    matrix[3][2] =
        position.z;

    animation.disk
        ->setLocalMatrix(matrix);

    if (completed)
    {
        animation.active =
            false;

        animation.disk =
            nullptr;

        animation.phase =
            AnimationPhase::NONE;
    }
}


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

    for (
        Eng::Node* child :
        node->getChildren()
        )
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


    /////////////////////
    // INIT HANOI GAME //
    /////////////////////

    game.init();

    if (!initHanoiNodes())
    {
        std::cerr
            << "Unable to initialize Hanoi scene"
            << std::endl;

        delete root;
        root = nullptr;

        eng.free();

        return -1;
    }

    // Visually select first rod:
    updateRodSelection();

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


    ////////////
    // CAMERA //
    ////////////

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

    eng.addCamera(camera);


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
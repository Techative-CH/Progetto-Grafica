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

// Hanoi Game
#include "hanoiGame.h"


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

Eng::Node* root = nullptr;
Eng::List* renderList = nullptr;
Eng::Camera* camera = nullptr;

HanoiGame game;

std::vector<Eng::Node*> diskNodes;
std::vector<Eng::Node*> rodNodes;

const std::string rodNames[HanoiGame::NUM_RODS] =
{
    "Pole_Left",
    "Pole_Center",
    "Pole_Right"
};

////////////////////////
// CALLBACK FUNCTIONS //
////////////////////////

void displayCallback()
{
    Eng::Base& eng = Eng::Base::getInstance();

    eng.clearWindow();
    eng.loadIdentity();

    if (root != nullptr && renderList != nullptr)
    {
        renderList->pass(root);
        eng.render(renderList);
    }

    eng.swapBuffers();

    // Request next frame:
    eng.postRedisplay();
}


void reshapeCallback(int width, int height)
{
    Eng::Base& eng = Eng::Base::getInstance();

    eng.setViewport(0, 0, width, height);

    if (camera != nullptr)
    {
        camera->setPerspective(
            45.0f,
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            1000.0f
        );
    }
}


void keyboardCallback(unsigned char key, int mouseX, int mouseY)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    }

    Eng::Base::getInstance().postRedisplay();
}


void specialCallback(int key, int mouseX, int mouseY)
{
    std::cout
        << "Special key pressed: "
        << key
        << std::endl;
}

bool initHanoiNodes()
{
    diskNodes.clear();
    rodNodes.clear();

    // Rods
    for (int i = 0; i < HanoiGame::NUM_RODS; i++)
    {
        Eng::Node* rod = root->findByName(rodNames[i]);

        if (rod == nullptr)
        {
            std::cerr
                << "Unable to find rod: "
                << rodNames[i]
                << std::endl;

            return false;
        }

        rodNodes.push_back(rod);
    }

    // Disks
    for (int i = 1; i <= HanoiGame::NUM_DISKS; i++)
    {
        std::string diskName =
            "Disk_" + std::to_string(i);

        Eng::Node* disk = root->findByName(diskName);

        if (disk == nullptr)
        {
            std::cerr
                << "Unable to find disk: "
                << diskName
                << std::endl;

            return false;
        }

        diskNodes.push_back(disk);
    }

    return true;
}

void printNodePositions(Eng::Node* node, int depth = 0)
{
    if (!node)
        return;

    glm::mat4 world = node->getWorldMatrix();

    glm::vec3 position(
        world[3][0],
        world[3][1],
        world[3][2]
    );

    std::cout
        << std::string(depth * 2, ' ')
        << node->getName()
        << " -> "
        << position.x << ", "
        << position.y << ", "
        << position.z
        << std::endl;

    for (Eng::Node* child : node->getChildren())
        printNodePositions(child, depth + 1);
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
    std::cout
        << "Client - Tower of Hanoi, S. Banfi (C) SUPSI"
        << std::endl
        << std::endl;

    Eng::Base& eng = Eng::Base::getInstance();


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

    root = eng.load("assets/hanoi/hanoi.ovo");

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

    game.init();
    game.printState();

    ///////////////////////
    // BUILD RENDER LIST //
    ///////////////////////

    renderList = eng.buildList(root);

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
        << renderList->getElements().size()
        << std::endl;

    ////////////
    // CAMERA //
    ////////////

    camera = new Eng::Camera("MainCamera");

    glm::vec3 cameraPosition(-36.0f, 39.0f, 60.0f);
    glm::vec3 cameraTarget(-40.0f, 18.0f, 12.0f);

    glm::mat4 view = glm::lookAt(
        cameraPosition,
        cameraTarget,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    camera->setLocalMatrix(glm::inverse(view));

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

    eng.setDisplayCallback(displayCallback);
    eng.setReshapeCallback(reshapeCallback);
    eng.setKeyboardCallback(keyboardCallback);
    eng.setSpecialCallback(specialCallback);


    //////////////////////
    // START MAIN LOOP //
    //////////////////////

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
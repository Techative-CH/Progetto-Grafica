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

// C/C++:
#include <iostream>

// Hanoi Game
#include "hanoiGame.h"


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

Eng::Node* root = nullptr;
Eng::List* renderList = nullptr;


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
        // Rebuild the render list in case scene transforms change:
        renderList->pass(root);

        // Render loaded OVO scene:
        eng.render(renderList);
    }

    eng.swapBuffers();
}


void reshapeCallback(int width, int height)
{
    Eng::Base& eng = Eng::Base::getInstance();

    eng.setViewport(0, 0, width, height);

    eng.setPerspective(
        45.0f,
        static_cast<float>(width) / static_cast<float>(height),
        1.0f,
        1000.0f
    );
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
        640,
        480
    );

    eng.setPerspective(
        45.0f,
        640.0f / 480.0f,
        1.0f,
        1000.0f
    );

    eng.setBackgroundColor(
        0.0f,
        0.0f,
        0.0f
    );


    ////////////////////
    // LOAD OVO SCENE //
    ////////////////////

    root = eng.load("simple3dScene.ovo");

    if (root == nullptr)
    {
        std::cerr
            << "OVO loading failed"
            << std::endl;

        eng.free();

        return -1;
    }

    std::cout
        << "OVO loaded successfully"
        << std::endl;


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
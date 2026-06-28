/**
 * @file		main.cpp
 * @brief	Client application (that uses the graphics engine)
 *
 * @author	Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
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

Eng::Node* root = nullptr;
float angle = 0.0f;

void displayCallback()
{
	Eng::Base& eng = Eng::Base::getInstance();

	eng.clearWindow();
	eng.loadIdentity();

	if (root != nullptr)
		eng.render(root);

	eng.swapBuffers();
}

void reshapeCallback(int width, int height)
{
	Eng::Base& eng = Eng::Base::getInstance();

	eng.setViewport(0, 0, width, height);
	eng.setPerspective(45.0f, (float)width / (float)height, 1.0f, 100.0f);
}

void keyboardCallback(unsigned char key, int mouseX, int mouseY)
{
	std::cout << "Key pressed: " << key << std::endl;

	if (key == 27) // ESC
		exit(0);
}

void specialCallback(int key, int mouseX, int mouseY)
{
	std::cout << "Special key pressed: " << key << std::endl;
}

void timerCallback(int value)
{
	angle += 1.0f;

	if (root != nullptr && !root->getChildren().empty())
		root->getChildren()[0]->setRotation(angle, 0.0f, 1.0f, 0.0f);

	Eng::Base& eng = Eng::Base::getInstance();
	eng.postRedisplay();
	eng.setTimerCallback(16, timerCallback, 0);
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
	std::cout << "Client - Tower of Hanoi, S. Banfi (C) SUPSI" << std::endl;
	std::cout << std::endl;

	Eng::Base& eng = Eng::Base::getInstance();

	eng.init("Tower of Hanoi", 640, 480);
	eng.setPerspective(45.0f, 640.0f / 480.0f, 1.0f, 100.0f);
	eng.setBackgroundColor(0.0f, 0.0f, 0.0f);

	root = new Eng::Node("root");

	Eng::Mesh* cube1 = new Eng::Mesh("cube1");
	cube1->setPosition(0.0f, 0.0f, -45.0f);
	cube1->setCubeEdge(20.0f);

	Eng::Mesh* cube2 = new Eng::Mesh("cube2");
	cube2->setPosition(18.0f, 0.0f, 0.0f);
	cube2->setScale(0.6f, 0.6f, 0.6f);
	cube2->setCubeEdge(20.0f);

	Eng::Mesh* cube3 = new Eng::Mesh("cube3");
	cube3->setPosition(18.0f, 0.0f, 0.0f);
	cube3->setScale(0.6f, 0.6f, 0.6f);
	cube3->setCubeEdge(20.0f);

	root->addChild(cube1);
	cube1->addChild(cube2);
	cube2->addChild(cube3);
	
	// Callbacks
	eng.setDisplayCallback(displayCallback);
	eng.setReshapeCallback(reshapeCallback);
	eng.setKeyboardCallback(keyboardCallback);
	eng.setSpecialCallback(specialCallback);
	eng.setTimerCallback(16, timerCallback, 0);

	std::cout << "Engine started correctly" << std::endl;

	eng.mainLoop();

	delete root;
	root = nullptr;

	eng.free();

	std::cout << "\n[application terminated]" << std::endl;
	return 0;
}
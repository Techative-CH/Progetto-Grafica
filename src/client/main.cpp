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

void displayCallback()
{
	Eng::Base& eng = Eng::Base::getInstance();

	eng.clearWindow();
	eng.swapBuffers();
}

void reshapeCallback(int width, int height)
{
	Eng::Base& eng = Eng::Base::getInstance();

	eng.setViewport(0, 0, width, height);
	std::cout << "Window resized: " << width << "x" << height << std::endl;
}

void keyboardCallback(unsigned char key, int mouseX, int mouseY)
{
	std::cout << "Key pressed: " << key << std::endl;

	if (key == 27) // ESC
		std::cout << "ESC pressed" << std::endl;
}

void specialCallback(int key, int mouseX, int mouseY)
{
	std::cout << "Special key pressed: " << key << std::endl;
}

void timerCallback(int value)
{
	std::cout << "Timer callback" << std::endl;

	Eng::Base& eng = Eng::Base::getInstance();
	eng.postRedisplay();

	eng.setTimerCallback(3000, timerCallback, 0);
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
	/*// Credits
	std::cout << "Client - Tower of Hanoi, S. Banfi (C) SUPSI" << std::endl;
	std::cout << std::endl;

	// Init engine
	Eng::Base& eng = Eng::Base::getInstance();

	eng.init("Tower of Hanoi", 640, 480);
	eng.setDisplayCallback(displayCallback);
	eng.setKeyboardCallback(keyboardCallback);
	eng.setSpecialCallback(specialCallback);

	eng.mainLoop();

	eng.free();

	HanoiGame game;
	game.init();

	std::cout << "\nStart:" << std::endl;
	game.printState();

	// Some manual move checks
	std::cout << "Move 0 -> 1: " << game.moveDisk(0, 1) << std::endl;
	std::cout << "Move 0 -> 1: " << game.moveDisk(0, 1) << " (should fail)" << std::endl;
	std::cout << "Move 2 -> 0: " << game.moveDisk(2, 0) << " (empty rod)" << std::endl;
	std::cout << "Move 0 -> 0: " << game.moveDisk(0, 0) << " (same rod)" << std::endl;

	std::cout << "\nAfter some moves:" << std::endl;
	game.printState();

	// Undo / redo check
	std::cout << "Undo last valid move" << std::endl;
	game.undo();
	game.printState();

	std::cout << "Redo last move" << std::endl;
	game.redo();
	game.printState();

	// Solve the puzzle on rod 2
	game.reset();

	game.moveDisk(0, 2);
	game.moveDisk(0, 1);
	game.moveDisk(2, 1);
	game.moveDisk(0, 2);
	game.moveDisk(1, 0);
	game.moveDisk(1, 2);
	game.moveDisk(0, 2);

	std::cout << "\nSolved state:" << std::endl;
	game.printState();
	std::cout << "Solved: " << game.isSolved() << std::endl;

	// Reset check
	game.reset();

	std::cout << "\nAfter reset:" << std::endl;
	game.printState();
	std::cout << "Solved: " << game.isSolved() << std::endl;

	// Release engine
	eng.free();

	std::cout << "\n[application terminated]" << std::endl;
	return 0;*/

	std::cout << "Client - Tower of Hanoi, S. Banfi (C) SUPSI" << std::endl;
	std::cout << std::endl;

	Eng::Base& eng = Eng::Base::getInstance();

	eng.init("Tower of Hanoi", 640, 480);
	
	// Callbacks
	eng.setDisplayCallback(displayCallback);
	eng.setReshapeCallback(reshapeCallback);
	eng.setKeyboardCallback(keyboardCallback);
	eng.setSpecialCallback(specialCallback);
	eng.setTimerCallback(3000, timerCallback, 0);

	std::cout << "Engine started correctly" << std::endl;

	eng.mainLoop();

	eng.free();

	std::cout << "\n[application terminated]" << std::endl;
	return 0;
}
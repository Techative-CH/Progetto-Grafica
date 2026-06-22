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
	// Credits:
	std::cout << "Client - Tower of Hanoi, S. Banfi (C) SUPSI" << std::endl;
	std::cout << std::endl;

	// Init engine:
	Eng::Base& eng = Eng::Base::getInstance();
	eng.init();

	// Create game:
	HanoiGame game;

	std::cout << "Before init:" << std::endl;
	game.printState();

	game.init();

	std::cout << "\nAfter init:" << std::endl;
	game.printState();

	std::cout << "\nMove 0 -> 1: " << game.moveDisk(0, 1) << std::endl;
	game.printState();

	std::cout << "\nMove 0 -> 1 again (should be invalid): " << game.moveDisk(0, 1) << std::endl;
	game.printState();

	std::cout << "\nMove 0 -> 2: " << game.moveDisk(0, 2) << std::endl;
	game.printState();

	std::cout << "\nMove 1 -> 2: " << game.moveDisk(1, 2) << std::endl;
	game.printState();

	std::cout << "\nMove from empty rod 1 -> 0 (should be invalid): " << game.moveDisk(1, 0) << std::endl;
	game.printState();

	std::cout << "\nMove same rod 0 -> 0 (should be invalid): " << game.moveDisk(0, 0) << std::endl;
	game.printState();

	std::cout << "\nMove invalid rod -1 -> 2 (should be invalid): " << game.moveDisk(-1, 2) << std::endl;
	game.printState();

	game.reset();

	std::cout << "\nAfter reset:" << std::endl;
	game.printState();

	// Release engine:
	eng.free();

	// Done:
	std::cout << "\n[application terminated]" << std::endl;
	return 0;
}
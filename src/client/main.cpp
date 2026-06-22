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
	// Credits
	std::cout << "Client - Tower of Hanoi, S. Banfi (C) SUPSI" << std::endl;
	std::cout << std::endl;

	// Init engine
	Eng::Base& eng = Eng::Base::getInstance();
	eng.init();

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
	return 0;
}
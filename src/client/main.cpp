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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C/C++:
#include <iostream>

// Hanoi Game
#include "hanoiGame.h"

Eng::Node* root = nullptr;
Eng::List* renderList = nullptr;
Eng::Mesh* cube1 = nullptr;
Eng::Texture* checkerboard = nullptr;
Eng::Material* checkerMaterial = nullptr;

int currentFilter = 0;
bool anisotropicFiltering = false;
bool repeatWrapping = true;
float angle = 0.0f;

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
}

void reshapeCallback(int width, int height)
{
	Eng::Base& eng = Eng::Base::getInstance();

	eng.setViewport(0, 0, width, height);
	eng.setPerspective(45.0f, (float)width / (float)height, 1.0f, 100.0f);
}

void keyboardCallback(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;

	case 'f':
		if (checkerboard == nullptr)
			break;

		currentFilter = (currentFilter + 1) % 5;

		switch (currentFilter)
		{
		case 0:
			checkerboard->setFilter(Eng::TextureFilter::NEAREST);
			std::cout << "Nearest" << std::endl;
			break;

		case 1:
			checkerboard->setFilter(Eng::TextureFilter::LINEAR);
			std::cout << "Linear" << std::endl;
			break;

		case 2:
			checkerboard->setFilter(Eng::TextureFilter::NEAREST_MIPMAP);
			std::cout << "Nearest mipmap" << std::endl;
			break;

		case 3:
			checkerboard->setFilter(Eng::TextureFilter::BILINEAR_MIPMAP);
			std::cout << "Bilinear mipmap" << std::endl;
			break;

		case 4:
			checkerboard->setFilter(Eng::TextureFilter::TRILINEAR);
			std::cout << "Trilinear" << std::endl;
			break;
		}
		
		break;

	case 'w':
		if (checkerboard == nullptr)
			break;

		repeatWrapping = !repeatWrapping;

		if (repeatWrapping) 
		{
			checkerboard->setWrap(Eng::TextureWrap::REPEAT);
		}
		else {
			checkerboard->setWrap(Eng::TextureWrap::CLAMP);
		}

		break;

	case 'a':
		if (checkerboard == nullptr)
			break;

		anisotropicFiltering = !anisotropicFiltering;

		if (anisotropicFiltering)
		{
			checkerboard->setFilter(
				Eng::TextureFilter::TRILINEAR
			);

			checkerboard->setAnisotropy(
				checkerboard->getMaxAnisotropy()
			);

			std::cout
				<< "Anisotropic filtering: "
				<< checkerboard->getAnisotropy()
				<< "x"
				<< std::endl;
		}
		else
		{
			checkerboard->setAnisotropy(1.0f);

			std::cout
				<< "Anisotropic filtering disabled"
				<< std::endl;
		}

		break;
	}

	Eng::Base::getInstance().postRedisplay();
}

void specialCallback(int key, int mouseX, int mouseY)
{
	std::cout << "Special key pressed: " << key << std::endl;
}

void timerCallback(int value)
{
	angle += 0.2f;

	if (cube1 != nullptr)
	{
		glm::mat4 matrix{ 1.0f };

		matrix = glm::translate(
			matrix,
			glm::vec3(0.0f, 0.0f, -45.0f)
		);

		matrix = glm::rotate(
			matrix,
			glm::radians(angle),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		cube1->setLocalMatrix(matrix);
	}

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

	cube1 = new Eng::Mesh("cube1");
	glm::mat4 cube1Matrix{ 1.0f };

	cube1Matrix = glm::translate(
		cube1Matrix,
		glm::vec3(0.0f, 0.0f, -45.0f)
	);

	cube1->setLocalMatrix(cube1Matrix);
	cube1->setCubeEdge(20.0f);

	Eng::Mesh* cube2 = new Eng::Mesh("cube2");
	glm::mat4 cube2Matrix{ 1.0f };

	cube2Matrix = glm::translate(
		cube2Matrix,
		glm::vec3(18.0f, 0.0f, 0.0f)
	);

	cube2Matrix = glm::scale(
		cube2Matrix,
		glm::vec3(0.6f)
	);

	cube2->setLocalMatrix(cube2Matrix);
	cube2->setCubeEdge(20.0f);

	Eng::Mesh* cube3 = new Eng::Mesh("cube3");
	glm::mat4 cube3Matrix{ 1.0f };

	cube3Matrix = glm::translate(
		cube3Matrix,
		glm::vec3(18.0f, 0.0f, 0.0f)
	);

	cube3Matrix = glm::scale(
		cube3Matrix,
		glm::vec3(0.6f)
	);

	cube3->setLocalMatrix(cube3Matrix);
	cube3->setCubeEdge(20.0f);

	Eng::Light* light = new Eng::Light("light");
	glm::mat4 lightMatrix{ 1.0f };

	lightMatrix = glm::translate(
		lightMatrix,
		glm::vec3(0.0f, 20.0f, -20.0f)
	);

	light->setLocalMatrix(lightMatrix);
	light->setColor(1.0f, 1.0f, 1.0f);

	root->addChild(cube1);
	cube1->addChild(cube2);
	cube2->addChild(cube3);
	root->addChild(light);

	renderList = eng.buildList(root);

	std::cout << "Render elements: "
		<< renderList->getElements().size()
		<< std::endl;
	
	// Callbacks
	eng.setDisplayCallback(displayCallback);
	eng.setReshapeCallback(reshapeCallback);
	eng.setKeyboardCallback(keyboardCallback);
	eng.setSpecialCallback(specialCallback);
	eng.setTimerCallback(16, timerCallback, 0);

	checkerboard = new Eng::Texture("checkerboard");

	if (!checkerboard->createCheckerboard(256, 256))
	{
		std::cout << "Unable to create texture" << std::endl;
	}

	checkerMaterial = new Eng::Material("checkerMaterial");
	checkerMaterial->setTexture(checkerboard);
	checkerMaterial->setAmbient(0.05f, 0.05f, 0.05f);
	checkerMaterial->setDiffuse(0.7f, 0.7f, 0.7f);
	checkerMaterial->setSpecular(1.0f, 1.0f, 1.0f);
	checkerMaterial->setShininess(64.0f);
	checkerMaterial->setEmission(0.0f, 0.0f, 0.0f);

	cube1->setMaterial(checkerMaterial);
	cube2->setMaterial(checkerMaterial);
	cube3->setMaterial(checkerMaterial);

	std::cout << "Engine started correctly" << std::endl;

	eng.mainLoop();

	delete renderList;
	renderList = nullptr;

	delete root;
	root = nullptr;

	delete checkerMaterial;
	checkerMaterial = nullptr;

	delete checkerboard;
	checkerboard = nullptr;

	eng.free();

	std::cout << "\n[application terminated]" << std::endl;
	return 0;
}
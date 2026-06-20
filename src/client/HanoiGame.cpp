#include "HanoiGame.h"
#include <iostream>

void HanoiGame::init() 
{
    if (initialized)
        return;

    initialized = true;

    reset();
}

void HanoiGame::reset() 
{
    for (int i = 0; i < NUM_RODS; i++) 
    {
        rods[i].clear();
    }

    for (int i = NUM_DISKS; i >= 1; i--) 
    {
        rods[0].push_back(i);
    }
}

void HanoiGame::printState() const
{
    for (int i = 0; i < NUM_RODS; i++)
    {
        std::cout << "Rod " << i << ": ";

        for (int disk : rods[i])
        {
            std::cout << disk << " ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}
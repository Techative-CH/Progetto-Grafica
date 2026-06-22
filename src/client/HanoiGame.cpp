#include "HanoiGame.h"
#include <iostream>

void HanoiGame::init() 
{
    if (initialized)
        return;

    initialized = true;

    reset();
}

bool HanoiGame::isMoveValid(int from, int to) const
{
    if (from < 0 || from >= NUM_RODS)
        return false;

    if (to < 0 || to >= NUM_RODS)
        return false;

    if (from == to)
        return false;

    if (rods[from].empty())
        return false;

    if (rods[to].empty())
        return true;

    int lastFrom = rods[from].back();
    int lastTo = rods[to].back();

    return lastFrom < lastTo;
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
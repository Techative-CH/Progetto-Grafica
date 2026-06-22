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

    int lastDiskFrom = rods[from].back();
    int lastDiskTo = rods[to].back();

    return lastDiskFrom < lastDiskTo;
}

bool HanoiGame::moveDisk(int from, int to)
{
    if (!isMoveValid(from, to))
        return false;

    int disk = rods[from].back();
    rods[from].pop_back();

    rods[to].push_back(disk);

    return true;
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

bool HanoiGame::isSolved() const
{
    for (int i = 1; i < NUM_RODS; i++)
    {
        if (rods[i].size() == NUM_DISKS)
            return true;
    }

    return false;
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
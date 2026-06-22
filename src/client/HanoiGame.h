#pragma once

#include <vector>

class HanoiGame
{
private:
    static const int NUM_RODS = 3;
    static const int NUM_DISKS = 3;

    std::vector<int> rods[NUM_RODS];

    bool initialized = false;

public:
    void init();

    bool isMoveValid(int from, int to) const;
    bool moveDisk(int from, int to);

    void undo();
    void redo();
    void reset();

    bool isSolved() const;

    void printState() const;
};
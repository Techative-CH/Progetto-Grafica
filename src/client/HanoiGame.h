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

    bool moveDisk(int from, int to);
    bool isMoveValid(int from, int to) const;

    void undo();
    void redo();
    void reset();

    bool isSolved() const;

    void printState() const;
};


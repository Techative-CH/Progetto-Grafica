#pragma once

#include <vector>
#include "move.h"

class HanoiGame
{
public:
    static const int NUM_RODS = 3;
    static const int NUM_DISKS = 5;

    void init();

    bool isMoveValid(int from, int to) const;
    bool moveDisk(int from, int to);

    void undo();
    void redo();
    void reset();

    bool isSolved() const;

    void printState() const;

private:
    std::vector<int> rods[NUM_RODS];
    std::vector<Move> undoStack;
    std::vector<Move> redoStack;

    bool initialized = false;
};
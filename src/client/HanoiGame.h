#pragma once
#include <vector>
class HanoiGame
{
private:
	std::vector<int> rods[3];

public:
    bool moveDisk(int from, int to);
    bool isMoveValid(int from, int to);
    void undo();
    void redo();
    void reset();
    bool isSolved();
};


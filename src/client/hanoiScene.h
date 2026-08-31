#pragma once

#include "engine.h"
#include "hanoiGame.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>


enum class AnimationPhase
{
    NONE,
    LIFT,
    HORIZONTAL,
    DROP
};


struct DiskAnimation
{
    bool active = false;

    Eng::Node* disk = nullptr;

    glm::vec3 targetPosition;

    float liftHeight = 0.0f;

    AnimationPhase phase = AnimationPhase::NONE;
};


class HanoiScene
{
public:
    bool init(Eng::Node* root);

    void update(float deltaTime);

    void updateRodSelection(int selectedRod);

    bool selectDisk(int disk);
    void cancelDiskSelection();

    void moveDisk(
        int disk,
        int destinationRod,
        int destinationLevel
    );

    void reset();

    bool isAnimating() const;

private:
    std::vector<Eng::Node*> diskNodes;
    std::vector<Eng::Node*> rodNodes;

    std::vector<glm::mat4> diskOriginalMatrices;
    std::vector<glm::mat4> rodOriginalMatrices;

    Eng::Node* selectedDiskNode = nullptr;
    glm::vec3 selectedDiskOriginalPosition;

    DiskAnimation animation;

    float diskSpacing = 0.0f;
    float baseDiskHeight = 0.0f;

    static constexpr float DISK_SPEED = 150.0f;
    static constexpr float LIFT_MARGIN = 15.0f;
    static constexpr float SELECTION_HEIGHT = 4.0f;
    static constexpr float ROD_SELECTION_HEIGHT = 4.0f;
};
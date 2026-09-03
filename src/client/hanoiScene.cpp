#include "hanoiScene.h"

#include <iostream>


bool HanoiScene::init(Eng::Node* root)
{
    if (root == nullptr)
        return false;

    diskNodes.clear();
    rodNodes.clear();
    rodOriginalMatrices.clear();
    diskOriginalMatrices.clear();

    const std::string rodNames[HanoiGame::NUM_RODS] =
    {
        "Pole_Left",
        "Pole_Center",
        "Pole_Right"
    };

    // Rods
    for (int i = 0; i < HanoiGame::NUM_RODS; i++)
    {
        Eng::Node* rod =
            root->findByName(rodNames[i]);

        if (rod == nullptr)
        {
            std::cerr << "[HANOI ERROR] Unable to find rod: " << rodNames[i] << std::endl;
            return false;
        }

        rodNodes.push_back(rod);

        rodOriginalMatrices.push_back(
            rod->getLocalMatrix()
        );
    }

    // Disks
    if (HanoiGame::NUM_DISKS < 2)
    {
        std::cerr << "[HANOI ERROR] At least two disks are required" << std::endl;
        return false;
    }

    for (int i = 1; i <= HanoiGame::NUM_DISKS; i++)
    {
        std::string diskName = "Disk_" + std::to_string(i);

        Eng::Node* disk = root->findByName(diskName);

        if (disk == nullptr)
        {
            std::cerr << "[HANOI ERROR] Unable to find disk: " << diskName << std::endl;
            return false;
        }

        diskNodes.push_back(disk);

        diskOriginalMatrices.push_back(
            disk->getLocalMatrix()
        );
    }

    // Disk height and spacing
    Eng::Node* disk1 = diskNodes[HanoiGame::NUM_DISKS - 1];
    Eng::Node* disk2 = diskNodes[HanoiGame::NUM_DISKS - 2];

    glm::vec3 bottomPosition = glm::vec3(
        disk1->getLocalMatrix()[3]
    );

    glm::vec3 nextPosition = glm::vec3(
        disk2->getLocalMatrix()[3]
    );

    baseDiskHeight = bottomPosition.y;
    diskSpacing = nextPosition.y - bottomPosition.y;

    return true;
}

void HanoiScene::update(float deltaTime)
{
    if (!animation.active || animation.disk == nullptr)
        return;

    glm::mat4 matrix = animation.disk->getLocalMatrix();
    glm::vec3 position = glm::vec3(matrix[3]);

    float movement = DISK_SPEED * deltaTime;
    bool completed = false;

    switch (animation.phase)
    {
    case AnimationPhase::LIFT:
    {
        position.y += movement;

        if (position.y >= animation.liftHeight)
        {
            position.y = animation.liftHeight;
            animation.phase = AnimationPhase::HORIZONTAL;
        }

        break;
    }

    case AnimationPhase::HORIZONTAL:
    {
        // Position over the target rod
        glm::vec3 horizontalTarget(
            animation.targetPosition.x,
            animation.liftHeight,
            animation.targetPosition.z
        );

        glm::vec3 difference = horizontalTarget - position;
        float distance = glm::length(difference);

        if (distance <= movement)
        {
            position = horizontalTarget;
            animation.phase = AnimationPhase::DROP;
        }
        else
        {
            position += glm::normalize(difference) * movement;
        }

        break;
    }

    case AnimationPhase::DROP:
    {
        position.y -= movement;

        if (position.y <= animation.targetPosition.y)
        {
            position = animation.targetPosition;
            completed = true;
        }

        break;
    }

    case AnimationPhase::NONE:
        return;
    }

    // Set new position
    matrix[3][0] = position.x;
    matrix[3][1] = position.y;
    matrix[3][2] = position.z;

    animation.disk->setLocalMatrix(matrix);

    if (completed)
    {
        animation.active = false;
        animation.disk = nullptr;
        animation.phase = AnimationPhase::NONE;
    }
}

void HanoiScene::updateRodSelection(int selectedRod)
{
    if (rodNodes.size() != HanoiGame::NUM_RODS)
        return;

    for (int i = 0; i < HanoiGame::NUM_RODS; i++)
    {
        glm::mat4 matrix = rodOriginalMatrices[i];

        if (i == selectedRod)
            matrix[3][1] += ROD_SELECTION_HEIGHT;

        rodNodes[i]->setLocalMatrix(matrix);
    }
}

bool HanoiScene::selectDisk(int disk)
{
    if (disk < 1 || disk > HanoiGame::NUM_DISKS)
        return false;

    selectedDiskNode = diskNodes[disk - 1];

    if (selectedDiskNode == nullptr)
        return false;

    glm::mat4 matrix = selectedDiskNode->getLocalMatrix();
    selectedDiskOriginalPosition = glm::vec3(matrix[3]);

    matrix[3][1] += SELECTION_HEIGHT;

    selectedDiskNode->setLocalMatrix(matrix);

    return true;
}

void HanoiScene::cancelDiskSelection()
{
    if (selectedDiskNode == nullptr)
        return;

    glm::mat4 matrix = selectedDiskNode->getLocalMatrix();

    matrix[3][0] = selectedDiskOriginalPosition.x;
    matrix[3][1] = selectedDiskOriginalPosition.y;
    matrix[3][2] = selectedDiskOriginalPosition.z;

    selectedDiskNode->setLocalMatrix(matrix);

    selectedDiskNode = nullptr;
}

void HanoiScene::moveDisk(int disk, int destinationRod, int destinationLevel)
{
    if (disk < 1 || disk > HanoiGame::NUM_DISKS)
        return;

    if (destinationRod < 0 || destinationRod >= HanoiGame::NUM_RODS)
        return;

    if (destinationLevel < 0)
        return;

    Eng::Node* diskNode = diskNodes[disk - 1];

    if (diskNode == nullptr)
        return;

    glm::mat4 rodMatrix = rodOriginalMatrices[destinationRod];
    glm::vec3 rodPosition = glm::vec3(rodMatrix[3]);

    glm::vec3 targetPosition(
        rodPosition.x,
        baseDiskHeight +
        diskSpacing * destinationLevel,
        rodPosition.z
    );

    float liftHeight = baseDiskHeight + diskSpacing * HanoiGame::NUM_DISKS + LIFT_MARGIN;

    animation.active = true;
    animation.disk = diskNode;
    animation.targetPosition = targetPosition;
    animation.liftHeight = liftHeight;
    animation.phase = AnimationPhase::LIFT;

    // Selected disk is controlled by animation
    selectedDiskNode = nullptr;
}

void HanoiScene::reset()
{
    animation.active = false;
    animation.disk = nullptr;
    animation.phase = AnimationPhase::NONE;

    selectedDiskNode = nullptr;

    for (int i = 0; i < HanoiGame::NUM_DISKS; i++)
    {
        diskNodes[i]->setLocalMatrix(diskOriginalMatrices[i]);
    }
}

bool HanoiScene::isAnimating() const
{
    return animation.active;
}

void HanoiScene::renderControls()
{
    const int x = 10;
    const int bottomMargin = 15;
    const int spacing = 18;

    auto& eng = Eng::Base::getInstance();

    if (!showControls)
    {
        // Show help command
        eng.renderText("[H] Show help", x, bottomMargin);
        return;
    }

    int y = bottomMargin;

    // Hide help command
    eng.renderText("[H] Hide help", x, y);
    y += spacing;

    // Show exit command
    eng.renderText("[ESC] Exit", x, y);
    y += spacing;

    // Show shading command
    eng.renderText("[S] Flat / Gouraud", x, y);
    y += spacing;

    // Show wireframe command
    eng.renderText("[W] Wireframe", x, y);
    y += spacing;

    // Show secondary camera movement commands
    eng.renderText("[A / D] Move secondary camera", x, y);
    y += spacing;

    // Show camera command
    eng.renderText("[C] Switch camera", x, y);
    y += spacing;

    // Show undo / redo move commands
    eng.renderText("[Z / Y] Undo / Redo", x, y);
    y += spacing;

    // Show reset game command
    eng.renderText("[R] Reset", x, y);
    y += spacing;

    // Show pick disk command
    eng.renderText("[SPACE] Pick disk", x, y);
    y += spacing;

    // Show select rod commands
    eng.renderText("[LEFT / RIGHT] Select rod", x, y);
    y += spacing;

    // Show controls label
    eng.renderText("CONTROLS", x, y);
}

void HanoiScene::toggleControls()
{
    showControls = !showControls;
}

void HanoiScene::renderVictory(int viewportWidth, int viewportHeight)
{
    Eng::Base& eng = Eng::Base::getInstance();

    const std::string title = "TOWER OF HANOI SOLVED!";
    const std::string restart = "Press R to restart";

    const int characterWidth = 8;
    const int spacing = 20;

    int centerY = viewportHeight / 2;

    int titleX = (viewportWidth - static_cast<int>(title.length()) * characterWidth) / 2;
    int restartX = (viewportWidth - static_cast<int>(restart.length()) * characterWidth) / 2;

    eng.renderText(title, titleX, centerY + spacing);
    eng.renderText(restart, restartX, centerY - spacing);
}
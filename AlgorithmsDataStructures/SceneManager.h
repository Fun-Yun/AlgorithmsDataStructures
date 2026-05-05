#pragma once
#include "MazeScene.h"
#include <vector>

class MazeSceneManager
{
public:
    std::vector<MazeScene*> mazeScenes;
    int activeSceneIndex;

    MazeSceneManager();
    ~MazeSceneManager();

    void UpdateScene();
    void DrawScene();
    void SetActiveScene(int index);
    void ResetScene();

    void IncreaseSpeed();
    void DecreaseSpeed();
    int GetAlgorithmStepsPerFrame() const;

private:
    int algorithmStepsPerFrame;
};

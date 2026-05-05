#pragma once
#include "SortScene.h"
#include <vector>

class SortSceneManager
{
public:
    std::vector<SortScene*> sortingScenes;
    int activeSceneIndex;

    SortSceneManager();
    ~SortSceneManager();

    void UpdateScene();
    void DrawScene();
    void SetActiveScene(int index);
    void ResetScene();

    void IncreaseSpeed();
    void DecreaseSpeed();
    int GetAlgorithmStepsPerFrame() const;

private:
    int algorithmStepsPerFrame;

    std::vector<int> baseBarValues;
    void CreateSharedValues();
    void ApplyValuesToAllSortingScenes(bool alsoResetScenes);
};

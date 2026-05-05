#include "SortingManager.h"
#include "raylib.h"

SortSceneManager::SortSceneManager()
{
    sortingScenes.push_back(new BubbleSortScene());
    sortingScenes.push_back(new SelectionSortScene());
    sortingScenes.push_back(new HeapSortScene());
    activeSceneIndex = 0;

    algorithmStepsPerFrame = 1;

    CreateSharedValues();
    ApplyValuesToAllSortingScenes(true);
}

SortSceneManager::~SortSceneManager()
{
    for (int sceneIndex = 0; sceneIndex < sortingScenes.size(); sceneIndex++)
        delete sortingScenes[sceneIndex];
    sortingScenes.clear();
}

void SortSceneManager::CreateSharedValues()
{
    int count = 80;
    if (!sortingScenes.empty())
        count = (int)sortingScenes[0]->sortBars.barValues.size();

    baseBarValues.resize(count);

    SetRandomSeed((unsigned int)(GetTime() * 100000.0));
    for (int i = 0; i < count; i++)
    {
        baseBarValues[i] = GetRandomValue(10, 500);
    }
}

void SortSceneManager::ApplyValuesToAllSortingScenes(bool alsoResetScenes)
{
    for (int i = 0; i < (int)sortingScenes.size(); i++)
    {
        sortingScenes[i]->LoadBarValues(baseBarValues);
        if (alsoResetScenes) sortingScenes[i]->ResetScene();
    }
}

void SortSceneManager::UpdateScene()
{
    for (int stepIndex = 0; stepIndex < algorithmStepsPerFrame; stepIndex++)
        sortingScenes[activeSceneIndex]->UpdateScene();
}

void SortSceneManager::DrawScene()
{
    sortingScenes[activeSceneIndex]->DrawScene();
}

void SortSceneManager::SetActiveScene(int index)
{
    if (index < 0 || index >= sortingScenes.size()) return;
    activeSceneIndex = index;

    sortingScenes[activeSceneIndex]->LoadBarValues(baseBarValues);
    sortingScenes[activeSceneIndex]->ResetScene();
}

void SortSceneManager::ResetScene()
{
    CreateSharedValues();
    ApplyValuesToAllSortingScenes(true);
}

void SortSceneManager::IncreaseSpeed()
{
    algorithmStepsPerFrame++;
    if (algorithmStepsPerFrame > 50) algorithmStepsPerFrame = 50;
}

void SortSceneManager::DecreaseSpeed()
{
    algorithmStepsPerFrame--;
    if (algorithmStepsPerFrame < 1) algorithmStepsPerFrame = 1;
}

int SortSceneManager::GetAlgorithmStepsPerFrame() const
{
    return algorithmStepsPerFrame;
}

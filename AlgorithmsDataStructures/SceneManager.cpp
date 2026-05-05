#include "SceneManager.h"

MazeSceneManager::MazeSceneManager()
{
    mazeScenes.push_back(new BFSVisualizer());
    mazeScenes.push_back(new DFSVisualizer());

    MazeSnapshot sharedLayout = mazeScenes[0]->GetMazeLayout();
    mazeScenes[1]->LoadMazeLayout(sharedLayout);

    activeSceneIndex = 0;

    algorithmStepsPerFrame = 1;
}

MazeSceneManager::~MazeSceneManager()
{
    for (int sceneIndex = 0; sceneIndex < mazeScenes.size(); sceneIndex++)
        delete mazeScenes[sceneIndex];
    mazeScenes.clear();
}

void MazeSceneManager::UpdateScene()
{
    for (int stepIndex = 0; stepIndex < algorithmStepsPerFrame; stepIndex++)
        mazeScenes[activeSceneIndex]->UpdateScene();
}

void MazeSceneManager::DrawScene()
{
    mazeScenes[activeSceneIndex]->DrawScene(algorithmStepsPerFrame);
}

void MazeSceneManager::SetActiveScene(int index)
{
    if (index < 0 || index >= mazeScenes.size()) return;
    activeSceneIndex = index;
}

void MazeSceneManager::ResetScene()
{
    mazeScenes[0]->ResetScene();
    MazeSnapshot sharedLayout = mazeScenes[0]->GetMazeLayout();

    // Keep alogrithms on same scene for comparison
    for (int sceneIndex = 0; sceneIndex < mazeScenes.size(); sceneIndex++)
        mazeScenes[sceneIndex]->LoadMazeLayout(sharedLayout);
}

void MazeSceneManager::IncreaseSpeed()
{
    algorithmStepsPerFrame++;
    if (algorithmStepsPerFrame > 50) algorithmStepsPerFrame = 50;
}

void MazeSceneManager::DecreaseSpeed()
{
    algorithmStepsPerFrame--;
    if (algorithmStepsPerFrame < 1) algorithmStepsPerFrame = 1;
}

int MazeSceneManager::GetAlgorithmStepsPerFrame() const
{
    return algorithmStepsPerFrame;
}

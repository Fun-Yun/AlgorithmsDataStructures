#include "SceneManager.h"
#include "SortingManager.h"
#include "raylib.h"

enum AppMode
{
    SortingMode = 0,
    MazeMode = 1
};

void HandleMazeInput(MazeSceneManager& mazeSceneManager)
{
    if (IsKeyPressed(KEY_ONE)) mazeSceneManager.SetActiveScene(0);
    if (IsKeyPressed(KEY_TWO)) mazeSceneManager.SetActiveScene(1);
    if (IsKeyPressed(KEY_R)) mazeSceneManager.ResetScene();
    if (IsKeyPressed(KEY_UP)) mazeSceneManager.IncreaseSpeed();
    if (IsKeyPressed(KEY_DOWN)) mazeSceneManager.DecreaseSpeed();
}

void HandleSortingInput(SortSceneManager& sortingSceneManager)
{
    if (IsKeyPressed(KEY_ONE)) sortingSceneManager.SetActiveScene(0);
    if (IsKeyPressed(KEY_TWO)) sortingSceneManager.SetActiveScene(1);
    if (IsKeyPressed(KEY_THREE)) sortingSceneManager.SetActiveScene(2);
    if (IsKeyPressed(KEY_R)) sortingSceneManager.ResetScene();
    if (IsKeyPressed(KEY_UP)) sortingSceneManager.IncreaseSpeed();
    if (IsKeyPressed(KEY_DOWN)) sortingSceneManager.DecreaseSpeed();
}

const char* GetSwitchModeText(AppMode currentAppMode)
{
    switch (currentAppMode)
    {
    case MazeMode:
        return "L = Switch to Sorting";
    case SortingMode:
        return "L = Switch to Maze";
    default:
        return "";
    }
}

int main()
{
    InitWindow(1280, 720, "Maze Pathfinding Visualizer");
    SetTargetFPS(60);

    MazeSceneManager mazeSceneManager;
    SortSceneManager sortingSceneManager;

    AppMode currentAppMode = SortingMode;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_L))
        {
            currentAppMode = currentAppMode == SortingMode ? MazeMode : SortingMode;
        }

        switch (currentAppMode)
        {
            case MazeMode:
                HandleMazeInput(mazeSceneManager);
                mazeSceneManager.UpdateScene();
                break;
            case SortingMode:
                HandleSortingInput(sortingSceneManager);
                sortingSceneManager.UpdateScene();
                break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentAppMode)
        {
        case MazeMode:
            mazeSceneManager.DrawScene();
            break;
        case SortingMode:
            sortingSceneManager.DrawScene();
            break;
        }

        const int screenWidth = 1280;
        const char* switchModeText = GetSwitchModeText(currentAppMode);
        const int switchModeTextSize = 20;
        const int switchModeTextRightMargin = 16;
        const int switchModeTextTopMargin = 40;
        
        int switchModeTextWidth = MeasureText(switchModeText, switchModeTextSize);
        int switchModeTextX = screenWidth - switchModeTextRightMargin - switchModeTextWidth;
        int switchModeTextY = switchModeTextTopMargin;
        
        DrawRectangle(switchModeTextX - 8, switchModeTextY - 4, switchModeTextWidth + 16, switchModeTextSize + 10, Fade(BLACK, 0.08f));
        DrawRectangleLines(switchModeTextX - 8, switchModeTextY - 4, switchModeTextWidth + 16, switchModeTextSize + 10, GRAY);
        DrawText(switchModeText, switchModeTextX, switchModeTextY, switchModeTextSize, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

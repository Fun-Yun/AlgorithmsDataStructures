#include "MazeScene.h"
#include "raylib.h"
#include <algorithm>

MazeScene::MazeScene(int rowCount, int columnCount)
{
    this->rowCount = rowCount;
    this->columnCount = columnCount;

    startCellIndex = 0;
    goalCellIndex = 0;

    visitedCellCount = 0;
    solutionPathLength = 0;

    solved = false;
    failed = false;

    mousePathIndex = 0;
    mouseMoveTimer = 0.0f;
}

int MazeScene::GridPositionToIndex(int row, int col) const
{
    return row * columnCount + col;
}

void MazeScene::IndexToGridPosition(int index, int& row, int& col) const
{
    row = index / columnCount;
    col = index % columnCount;
}

std::vector<int> MazeScene::GetNeighbours(int index) const
{
    std::vector<int> neighbours;
    int row = 0;
    int col = 0;
    IndexToGridPosition(index, row, col);

    // Only goes up,down,left,right
    if (row > 0) neighbours.push_back(GridPositionToIndex(row - 1, col));
    if (row < rowCount - 1) neighbours.push_back(GridPositionToIndex(row + 1, col));
    if (col > 0) neighbours.push_back(GridPositionToIndex(row, col - 1));
    if (col < columnCount - 1) neighbours.push_back(GridPositionToIndex(row, col + 1));
    return neighbours;
}

bool MazeScene::IsBlocked(int index) const
{
    return gridCellStates[index] == WallCell;
}

void MazeScene::MarkFrontier(int index)
{
    if (index == startCellIndex || index == goalCellIndex) return;
    if (gridCellStates[index] == OpenCell) gridCellStates[index] = FrontierCell;
}

void MazeScene::MarkVisited(int index)
{
    if (index == startCellIndex || index == goalCellIndex) return;
    if (gridCellStates[index] != WallCell) gridCellStates[index] = VisitedCell;
}

void MazeScene::MarkPath(int index)
{
    if (index == startCellIndex || index == goalCellIndex) return;
    if (gridCellStates[index] != WallCell) gridCellStates[index] = PathCell;
}

void MazeScene::BuildSolvableMaze()
{
    gridCellStates.assign(rowCount * columnCount, WallCell);

    startCellIndex = GridPositionToIndex(1, 1);
    goalCellIndex = GridPositionToIndex(rowCount - 2, columnCount - 2);

    SetRandomSeed((unsigned int)(GetTime() * 100000.0));

    std::vector<int> stack;
    stack.push_back(startCellIndex);
    gridCellStates[startCellIndex] = OpenCell;

    // Carve passages with depthfist backtracking maze gen
    while (!stack.empty())
    {
        int current = stack.back();
        int currentRow = 0;
        int currentCol = 0;
        IndexToGridPosition(current, currentRow, currentCol);

        std::vector<int> candidates;

        int rowOffsets[4] = { -2, 2, 0, 0 };
        int columnOffsets[4] = { 0, 0, -2, 2 };

        for (int directionIndex = 0; directionIndex < 4; directionIndex++)
        {
            int nextRow = currentRow + rowOffsets[directionIndex];
            int nextCol = currentCol + columnOffsets[directionIndex];
            if (nextRow <= 0 || nextCol <= 0 || nextRow >= rowCount - 1 || nextCol >= columnCount - 1) continue;

            int nextIndex = GridPositionToIndex(nextRow, nextCol);
            if (gridCellStates[nextIndex] == WallCell)
                candidates.push_back(nextIndex);
        }

        if (candidates.empty())
        {
            stack.pop_back();
            continue;
        }

        int next = candidates[GetRandomValue(0, (int)candidates.size() - 1)];
        int nextRow = 0;
        int nextCol = 0;
        IndexToGridPosition(next, nextRow, nextCol);

        int wallRow = (currentRow + nextRow) / 2;
        int wallCol = (currentCol + nextCol) / 2;
        gridCellStates[GridPositionToIndex(wallRow, wallCol)] = OpenCell;
        gridCellStates[next] = OpenCell;
        stack.push_back(next);
    }

    for (int row = 1; row < rowCount - 1; row++)
    {
        for (int col = 1; col < columnCount - 1; col++)
        {
            int cellIndex = GridPositionToIndex(row, col);
            if (cellIndex == startCellIndex || cellIndex == goalCellIndex) continue;
            if (gridCellStates[cellIndex] == WallCell && GetRandomValue(0, 99) < 12)
                gridCellStates[cellIndex] = OpenCell;
        }
    }

    gridCellStates[startCellIndex] = OpenCell;
    gridCellStates[goalCellIndex] = OpenCell;
}

void MazeScene::RebuildPath()
{
    path.clear();
    if (!solved) return;

    int currentPathNodeIndex = goalCellIndex;
    path.push_back(currentPathNodeIndex);

    while (currentPathNodeIndex != startCellIndex)
    {
        int parentNodeIndex = cameFrom[currentPathNodeIndex];
        if (parentNodeIndex < 0 || parentNodeIndex == currentPathNodeIndex)
            break;
        currentPathNodeIndex = parentNodeIndex;
        path.push_back(currentPathNodeIndex);
    }

    std::reverse(path.begin(), path.end());
    solutionPathLength = (int)path.size();

    for (int pathIndex = 0; pathIndex < path.size(); pathIndex++)
        MarkPath(path[pathIndex]);

    mousePathIndex = 0;
    mouseMoveTimer = 0.0f;
}

void MazeScene::UpdateMouseAnimation()
{
    if (!solved || path.empty()) return;
    if (mousePathIndex >= solutionPathLength - 1) return;

    mouseMoveTimer += GetFrameTime();
    if (mouseMoveTimer >= 0.08f)
    {
        mouseMoveTimer = 0.0f;
        mousePathIndex++;
    }
}

MazeSnapshot MazeScene::GetMazeLayout() const
{
    MazeSnapshot layout;
    layout.mazeCellStates = gridCellStates;
    layout.startCellIndex = startCellIndex;
    layout.goalCellIndex = goalCellIndex;
    return layout;
}

void MazeScene::LoadMazeLayout(const MazeSnapshot& layout)
{
    gridCellStates = layout.mazeCellStates;
    startCellIndex = layout.startCellIndex;
    goalCellIndex = layout.goalCellIndex;
    PrepareSearch();
}

void MazeScene::DrawHeader(int algorithmStepsPerFrame) const
{
    DrawText("Maze Pathfinding Visualizer", 20, 20, 32, BLACK);
    DrawText(TextFormat("Algorithm: %s", Name()), 20, 58, 22, DARKGRAY);
    DrawText("1 BFS  2 DFS  |  R Reset  |  Up/Down Speed", 20, 86, 20, BLACK);
    DrawText(TextFormat("Speed: %d", algorithmStepsPerFrame), 1280 - 220, 86, 20, BLACK);
}

void MazeScene::DrawStats(const char* statusText) const
{
    int panelY = 720 - 56;
    DrawText(TextFormat("Visited: %d", visitedCellCount), 20, panelY + 11, 20, DARKGREEN);
    DrawText(TextFormat("Path Length: %d", solutionPathLength), 220, panelY + 11, 20, DARKGREEN);
    DrawText(statusText, 470, panelY + 11, 20, MAROON);
}

void MazeScene::DrawScene(int algorithmStepsPerFrame) const
{
    DrawHeader(algorithmStepsPerFrame);

    int boardX = 20;
    int boardY = 130;
    int boardSize = 720 - 210;
    int cellSize = boardSize / rowCount;

    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < columnCount; col++)
        {
            int cellIndex = GridPositionToIndex(row, col);
            int cellX = boardX + col * cellSize;
            int cellY = boardY + row * cellSize;

            Color color = RAYWHITE;
            if (gridCellStates[cellIndex] == WallCell) color = DARKGRAY;
            if (gridCellStates[cellIndex] == FrontierCell) color = SKYBLUE;
            if (gridCellStates[cellIndex] == VisitedCell) color = BLUE;
            if (gridCellStates[cellIndex] == PathCell) color = GOLD;

            DrawRectangle(cellX, cellY, cellSize - 1, cellSize - 1, color);
        }
    }

    int startRow = 0;
    int startColumn = 0;
    int goalRow = 0;
    int goalColumn = 0;
    IndexToGridPosition(startCellIndex, startRow, startColumn);
    IndexToGridPosition(goalCellIndex, goalRow, goalColumn);

    int startCellX = boardX + startColumn * cellSize;
    int startCellY = boardY + startRow * cellSize;
    int goalCellX = boardX + goalColumn * cellSize;
    int goalCellY = boardY + goalRow * cellSize;

    DrawRectangle(startCellX, startCellY, cellSize - 1, cellSize - 1, GREEN);
    DrawRectangle(goalCellX, goalCellY, cellSize - 1, cellSize - 1, ORANGE);

    DrawCircle(goalCellX + cellSize / 2, goalCellY + cellSize / 2, (float)cellSize * 0.22f, YELLOW);

    if (solved && !path.empty())
    {
        int mouseCellIndex = path[mousePathIndex];
        int mouseRow = 0;
        int mouseCol = 0;
        IndexToGridPosition(mouseCellIndex, mouseRow, mouseCol);
        float mouseCenterX = (float)(boardX + mouseCol * cellSize + cellSize / 2);
        float mouseCenterY = (float)(boardY + mouseRow * cellSize + cellSize / 2);
        float mouseSize = (float)cellSize * 0.8f;

        Vector2 topPoint = { mouseCenterX, mouseCenterY - mouseSize * 0.5f };
        Vector2 bottomLeftPoint = { mouseCenterX - mouseSize * 0.45f, mouseCenterY + mouseSize * 0.4f };
        Vector2 bottomRightPoint = { mouseCenterX + mouseSize * 0.45f, mouseCenterY + mouseSize * 0.4f };
        DrawTriangle(topPoint, bottomLeftPoint, bottomRightPoint, LIGHTGRAY);
    }

    const char* statusText = "Searching...";
    if (solved) statusText = "Path found! Mouse is going to the cheese.";
    if (failed) statusText = "No path found (this should not happen).";
    DrawStats(statusText);
}

BFSVisualizer::BFSVisualizer() : MazeScene(25, 25)
{
    ResetScene();
}

void BFSVisualizer::ResetScene()
{
    BuildSolvableMaze();
    PrepareSearch();
}

void BFSVisualizer::PrepareSearch()
{
    cameFrom.assign(rowCount * columnCount, -1);
    visitedCells.assign(rowCount * columnCount, false);
    cellsToVisit.clear();
    path.clear();

    visitedCellCount = 0;
    solutionPathLength = 0;
    solved = false;
    failed = false;
    mousePathIndex = 0;
    mouseMoveTimer = 0.0f;

    for (int cellIndex = 0; cellIndex < gridCellStates.size(); cellIndex++)
        if (gridCellStates[cellIndex] != WallCell) gridCellStates[cellIndex] = OpenCell;

    // BFS starts by placing start cell in queue
    cellsToVisit.push_back(startCellIndex);
    visitedCells[startCellIndex] = true;
    cameFrom[startCellIndex] = startCellIndex;
    MarkFrontier(startCellIndex);
}

void BFSVisualizer::UpdateScene()
{
    if (solved || failed)
    {
        UpdateMouseAnimation();
        return;
    }

    if (cellsToVisit.empty())
    {
        failed = true;
        return;
    }

    int current = cellsToVisit.front();
    cellsToVisit.pop_front();
    visitedCellCount++;

    MarkVisited(current);

    if (current == goalCellIndex)
    {
        solved = true;
        RebuildPath();
        return;
    }

    // BFS explores level by level finding shortest path
    std::vector<int> neighbours = GetNeighbours(current);
    for (int neighbourIndex = 0; neighbourIndex < neighbours.size(); neighbourIndex++)
    {
        int next = neighbours[neighbourIndex];
        if (IsBlocked(next)) continue;
        if (visitedCells[next]) continue;

        visitedCells[next] = true;
        cameFrom[next] = current;
        cellsToVisit.push_back(next);
        MarkFrontier(next);
    }
}

const char* BFSVisualizer::Name() const
{
    return "Breadth-First Search (BFS)";
}

DFSVisualizer::DFSVisualizer() : MazeScene(25, 25)
{
    ResetScene();
}

void DFSVisualizer::ResetScene()
{
    BuildSolvableMaze();
    PrepareSearch();
}

void DFSVisualizer::PrepareSearch()
{
    cameFrom.assign(rowCount * columnCount, -1);
    visitedCells.assign(rowCount * columnCount, false);
    cellsToVisit.clear();
    path.clear();

    visitedCellCount = 0;
    solutionPathLength = 0;
    solved = false;
    failed = false;
    mousePathIndex = 0;
    mouseMoveTimer = 0.0f;

    for (int cellIndex = 0; cellIndex < gridCellStates.size(); cellIndex++)
        if (gridCellStates[cellIndex] != WallCell) gridCellStates[cellIndex] = OpenCell;

    cellsToVisit.push_back(startCellIndex);
    cameFrom[startCellIndex] = startCellIndex;
    MarkFrontier(startCellIndex);
}

void DFSVisualizer::UpdateScene()
{
    if (solved || failed)
    {
        UpdateMouseAnimation();
        return;
    }

    if (cellsToVisit.empty())
    {
        failed = true;
        return;
    }

    int current = cellsToVisit.back();
    cellsToVisit.pop_back();

    if (visitedCells[current]) return;
    visitedCells[current] = true;
    visitedCellCount++;

    MarkVisited(current);

    if (current == goalCellIndex)
    {
        solved = true;
        RebuildPath();
        return;
    }

    // DFS follows one branch as far as it can before backtracking
    std::vector<int> neighbours = GetNeighbours(current);
    for (int neighbourIndex = (int)neighbours.size() - 1; neighbourIndex >= 0; neighbourIndex--)
    {
        int next = neighbours[neighbourIndex];
        if (IsBlocked(next)) continue;
        if (visitedCells[next]) continue;

        if (cameFrom[next] == -1)
            cameFrom[next] = current;

        cellsToVisit.push_back(next);
        MarkFrontier(next);
    }
}

const char* DFSVisualizer::Name() const
{
    return "Depth-First Search (DFS)";
}

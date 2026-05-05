#pragma once
#include <deque>
#include <string>
#include <vector>

enum GridCellState
{
    OpenCell = 0,
    WallCell = 1,
    FrontierCell = 2,
    VisitedCell = 3,
    PathCell = 4
};

struct MazeSnapshot
{
    std::vector<GridCellState> mazeCellStates;
    int startCellIndex;
    int goalCellIndex;
};

class MazeScene
{
public:
    MazeScene(int rowCount = 25, int columnCount = 25);
    virtual ~MazeScene() = default;

    virtual void ResetScene() = 0;
    virtual void UpdateScene() = 0;
    virtual const char* Name() const = 0;

    void DrawScene(int algorithmStepsPerFrame) const;

    MazeSnapshot GetMazeLayout() const;

    void LoadMazeLayout(const MazeSnapshot& layout);

protected:
    int rowCount;
    int columnCount;

    int startCellIndex;
    int goalCellIndex;

    int visitedCellCount;
    int solutionPathLength;

    bool solved;
    bool failed;

    std::vector<GridCellState> gridCellStates;
    std::vector<int> cameFrom;
    std::vector<bool> visitedCells;

    std::vector<int> path;

    int mousePathIndex;
    float mouseMoveTimer;

    void BuildSolvableMaze();
    void RebuildPath();
    void UpdateMouseAnimation();
    void DrawHeader(int algorithmStepsPerFrame) const;
    void DrawStats(const char* statusText) const;

    std::vector<int> GetNeighbours(int index) const;
    int GridPositionToIndex(int row, int col) const;
    void IndexToGridPosition(int index, int& row, int& col) const;

    bool IsBlocked(int index) const;
    void MarkFrontier(int index);
    void MarkVisited(int index);
    void MarkPath(int index);

    virtual void PrepareSearch() = 0;
};

class BFSVisualizer : public MazeScene
{
public:
    BFSVisualizer();
    void ResetScene();
    void UpdateScene();
    const char* Name() const;

private:
    std::deque<int> cellsToVisit;
    void PrepareSearch();
};

class DFSVisualizer : public MazeScene
{
public:
    DFSVisualizer();
    void ResetScene();
    void UpdateScene();
    const char* Name() const;

private:
    std::vector<int> cellsToVisit;
    void PrepareSearch();
};

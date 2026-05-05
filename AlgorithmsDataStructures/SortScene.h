#pragma once
#include "BarArray.h"
#include <vector>

class SortScene
{
public:
    SortableBars sortBars;

    int sortingPassIndex;
    int comparisonIndex;

    bool sorted;

    int comparisonCount;
    int swapCount;

    SortScene(int count = 80);
    virtual ~SortScene() = default;

    virtual void UpdateScene() = 0;
    virtual void DrawScene() = 0;
    virtual void ResetScene() = 0;
    virtual const char* Name() const = 0;

    void LoadBarValues(const std::vector<int>& barValues);

    void DrawHeader(const char* sceneName) const;
    void DrawStats(int comparisonCount, int swapCount, bool sorted) const;
};

class BubbleSortScene : public SortScene
{
public:
    BubbleSortScene();
    void UpdateScene();
    void DrawScene();
    void ResetScene();
    const char* Name() const;

private:
    bool passHadSwap;
};

class SelectionSortScene : public SortScene
{
public:
    SelectionSortScene();
    void UpdateScene();
    void DrawScene();
    void ResetScene();
    const char* Name() const;

private:
    int minIndex;
    int searchIndex;
    bool waitingToSwap;
};

class HeapSortScene : public SortScene
{
public:
    HeapSortScene();
    void UpdateScene();
    void DrawScene();
    void ResetScene();
    const char* Name() const;

private:
    enum HeapStage
    {
        BuildingHeap = 0,
        ExtractingHeap = 1,
        HeapSortComplete = 2
    };

    HeapStage stage;
    int activeHeapSize;
    int buildIndex;
    int siftRoot;
    int heapExtractionIndex;

    void StartSiftDown(int root, int size);
    bool StepSiftDown(int size);
};

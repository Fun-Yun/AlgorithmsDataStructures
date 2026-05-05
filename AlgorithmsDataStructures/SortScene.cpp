#include "SortScene.h"
#include "raylib.h"

SortScene::SortScene(int count) : sortBars(count)
{
    sortingPassIndex = 0;
    comparisonIndex = 0;
    sorted = false;
    comparisonCount = 0;
    swapCount = 0;
}

void SortScene::LoadBarValues(const std::vector<int>& barValues)
{
    int count = (int)sortBars.barValues.size();
    int copyCount = (int)barValues.size();
    if (copyCount > count) copyCount = count;

    // Copy a shared starting array so different sorting scenes can be compared fairly.
    for (int i = 0; i < copyCount; i++)
        sortBars.barValues[i] = barValues[i];

    sortBars.ClearBarStates();
}

void SortScene::DrawHeader(const char* sceneName) const
{
    DrawText(TextFormat("Scene: %s", sceneName), 20, 20, 32, BLACK);
    DrawText("1 Bubble  2 Selection  3 Heap", 20, 58, 22, DARKGRAY);
    DrawText("R = Reset", 20, 86, 20, BLACK);
    DrawText("Up/Down = Speed", 180, 86, 20, BLACK);
}

void SortScene::DrawStats(int comparisonCount, int swapCount, bool sorted) const
{
    int panelY = 720 - 88;
    DrawText(TextFormat("Comparisons: %d", comparisonCount), 20, panelY + 14, 24, DARKGREEN);
    DrawText(TextFormat("Swaps: %d", swapCount), 320, panelY + 14, 24, DARKGREEN);
    if (sorted) DrawText("Sorted!", 560, panelY + 14, 24, DARKGREEN);
}

BubbleSortScene::BubbleSortScene() : SortScene(80)
{
    ResetScene();
}

void BubbleSortScene::ResetScene()
{
    sortBars.ClearBarStates();
    sortingPassIndex = 0;
    comparisonIndex = 0;
    sorted = false;
    comparisonCount = 0;
    swapCount = 0;
    passHadSwap = false;
}

void BubbleSortScene::UpdateScene()
{
    if (sorted) return;

    sortBars.ClearBarStates();

    // One comparison happens per frame-step so the process is easy to follow visually.
    int elementCount = sortBars.barValues.size();
    if (elementCount <= 1)
    {
        sorted = true;
        return;
    }

    if (comparisonIndex >= elementCount - sortingPassIndex - 1)
    {
        if (!passHadSwap)
        {
            sorted = true;
            for (int index = 0; index < elementCount; index++) sortBars.SetBarState(index, SortedBar);
            return;
        }

        sortingPassIndex++;
        comparisonIndex = 0;
        passHadSwap = false;    

        if (sortingPassIndex >= elementCount - 1)
        {
            sorted = true;
            for (int index = 0; index < elementCount; index++) sortBars.SetBarState(index, SortedBar);
            return;
        }
    }

    sortBars.SetBarState(comparisonIndex, ComparingBar);
    sortBars.SetBarState(comparisonIndex + 1, ComparingBar);
    comparisonCount++;

    if (sortBars.GetValueAt(comparisonIndex) > sortBars.GetValueAt(comparisonIndex + 1))
    {
        sortBars.SwapValues(comparisonIndex, comparisonIndex + 1);
        sortBars.SetBarState(comparisonIndex, SwappingBar);
        sortBars.SetBarState(comparisonIndex + 1, SwappingBar);
        swapCount++;
        passHadSwap = true;
    }

    comparisonIndex++;
}

void BubbleSortScene::DrawScene()
{
    DrawHeader(Name());
    sortBars.DrawBars(32, 130, 1280 - 64, 720 - 220);
    DrawStats(comparisonCount, swapCount, sorted);
}

const char* BubbleSortScene::Name() const
{
    return "Bubble Sort";
}

SelectionSortScene::SelectionSortScene() : SortScene(80)
{
    ResetScene();
}

void SelectionSortScene::ResetScene()
{
    sortBars.ClearBarStates();
    sortingPassIndex = 0;
    comparisonIndex = 0;
    sorted = false;
    comparisonCount = 0;
    swapCount = 0;

    minIndex = 0;
    searchIndex = 1;
    waitingToSwap = false;
}

void SelectionSortScene::UpdateScene()
{
    if (sorted) return;
    sortBars.ClearBarStates();

    int elementCount = sortBars.barValues.size();
    if (elementCount <= 1)
    {
        sorted = true;
        return;
    }

    if (sortingPassIndex >= elementCount - 1)
    {
        sorted = true;
        for (int index = 0; index < elementCount; index++) sortBars.SetBarState(index, SortedBar);
        return;
    }

    if (!waitingToSwap)
    {
        //Scan unsorted section to find smallest remaining valu
        if (searchIndex < elementCount)
        {
            sortBars.SetBarState(minIndex, ComparingBar);
            sortBars.SetBarState(searchIndex, ComparingBar);
            comparisonCount++;

            if (sortBars.GetValueAt(searchIndex) < sortBars.GetValueAt(minIndex))
                minIndex = searchIndex;

            searchIndex++;
            return;
        }
        waitingToSwap = true;
    }

    if (waitingToSwap)
    {
        // Move smallest value to its final spot
        if (minIndex != sortingPassIndex)
        {
            sortBars.SwapValues(sortingPassIndex, minIndex);
            sortBars.SetBarState(sortingPassIndex, SwappingBar);
            sortBars.SetBarState(minIndex, SwappingBar);
            swapCount++;
        }
        sortBars.SetBarState(sortingPassIndex, SortedBar);

        sortingPassIndex++;
        minIndex = sortingPassIndex;
        searchIndex = sortingPassIndex + 1;
        waitingToSwap = false;
    }
}

void SelectionSortScene::DrawScene()
{
    DrawHeader(Name());
    sortBars.DrawBars(32, 130, 1280 - 64, 720 - 220);
    DrawStats(comparisonCount, swapCount, sorted);
}

const char* SelectionSortScene::Name() const
{
    return "Selection Sort";
}

HeapSortScene::HeapSortScene() : SortScene(80)
{
    ResetScene();
}

void HeapSortScene::ResetScene()
{
    sortBars.ClearBarStates();
    sortingPassIndex = 0;
    comparisonIndex = 0;
    sorted = false;
    comparisonCount = 0;
    swapCount = 0;

    stage = BuildingHeap;
    activeHeapSize = sortBars.barValues.size();

    buildIndex = activeHeapSize / 2 - 1;

    heapExtractionIndex = activeHeapSize - 1;

    siftRoot = -1;
}

void HeapSortScene::StartSiftDown(int root, int size)
{
    siftRoot = root;
    sortingPassIndex = root;
    comparisonIndex = size;
}

bool HeapSortScene::StepSiftDown(int size)
{
    if (siftRoot < 0) return true;

    int largest = siftRoot;
    int left = 2 * siftRoot + 1;
    int right = 2 * siftRoot + 2;

    sortBars.ClearBarStates();
    sortBars.SetBarState(siftRoot, ComparingBar);

    if (left < size)
    {
        sortBars.SetBarState(left, ComparingBar);
        comparisonCount++;
        if (sortBars.GetValueAt(left) > sortBars.GetValueAt(largest)) largest = left;
    }

    if (right < size)
    {
        sortBars.SetBarState(right, ComparingBar);
        comparisonCount++;
        if (sortBars.GetValueAt(right) > sortBars.GetValueAt(largest)) largest = right;
    }

    if (largest == siftRoot)
    {
        siftRoot = -1;
        return true;
    }

    sortBars.SwapValues(siftRoot, largest);
    sortBars.SetBarState(siftRoot, SwappingBar);
    sortBars.SetBarState(largest, SwappingBar);
    swapCount++;
    siftRoot = largest;
    return false;
}

void HeapSortScene::UpdateScene()
{
    if (sorted) return;

    if (stage == BuildingHeap)
    {
        if (siftRoot >= 0)
        {
            if (StepSiftDown(activeHeapSize))
            {
                buildIndex--;
                if (buildIndex >= 0) StartSiftDown(buildIndex, activeHeapSize);
                else stage = ExtractingHeap;
            }
        }
        else
        {
            if (buildIndex >= 0) StartSiftDown(buildIndex, activeHeapSize);
            else stage = ExtractingHeap;
        }
        return;
    }

    if (stage == ExtractingHeap)
    {
        sortBars.ClearBarStates();
        if (heapExtractionIndex <= 0)
        {
            stage = HeapSortComplete;
            sorted = true;
            for (int index = 0; index < sortBars.barValues.size(); index++) sortBars.SetBarState(index, SortedBar);
            return;
        }

        if (siftRoot < 0)
        {
            sortBars.SwapValues(0, heapExtractionIndex);
            sortBars.SetBarState(heapExtractionIndex, SortedBar);
            sortBars.SetBarState(0, SwappingBar);
            swapCount++;
            activeHeapSize = heapExtractionIndex;
            heapExtractionIndex--;
            StartSiftDown(0, activeHeapSize);
            return;
        }

        StepSiftDown(activeHeapSize);
        return;
    }

    if (stage == HeapSortComplete)
    {
        sorted = true;
    }
}

void HeapSortScene::DrawScene()
{
    DrawHeader(Name());
    sortBars.DrawBars(32, 130, 1280 - 64, 720 - 220);
    DrawStats(comparisonCount, swapCount, sorted);
}

const char* HeapSortScene::Name() const
{
    return "Heap Sort";
}

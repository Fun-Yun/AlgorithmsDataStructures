#pragma once
#include "raylib.h"
#include <vector>

enum BarVisualState
{
    NormalBar = 0,
    ComparingBar = 1,
    SwappingBar = 2,
    SortedBar = 3
};

class SortableBars
{
public:
    std::vector<int> barValues;
    std::vector<BarVisualState> barStates;

    SortableBars(int count = 80);

    void GenerateRandomValues();
    int GetValueAt(int index) const;
    void SwapValues(int leftIndex, int rightIndex);
    void SetBarState(int index, BarVisualState state);
    void ClearBarStates();
    void DrawBars(float chartX, float chartY, float chartWidth, float chartHeight) const;
};

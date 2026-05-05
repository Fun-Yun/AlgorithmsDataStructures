#include "BarArray.h"
#include <algorithm>

SortableBars::SortableBars(int count)
{
    barValues.resize(count);
    barStates.resize(count, NormalBar);
    GenerateRandomValues();
}

void SortableBars::GenerateRandomValues()
{
    for (int index = 0; index < barValues.size(); index++)
    {
        barValues[index] = GetRandomValue(10, 500);
        barStates[index] = NormalBar;
    }
}

int SortableBars::GetValueAt(int index) const
{
    return barValues[index];
}

void SortableBars::SwapValues(int leftIndex, int rightIndex)
{
    int temporaryValue = barValues[leftIndex];
    barValues[leftIndex] = barValues[rightIndex];
    barValues[rightIndex] = temporaryValue;
}

void SortableBars::SetBarState(int index, BarVisualState state)
{
    barStates[index] = state;
}

void SortableBars::ClearBarStates()
{
    for (int index = 0; index < barStates.size(); index++)
        barStates[index] = NormalBar;
}

void SortableBars::DrawBars(float chartX, float chartY, float chartWidth, float chartHeight) const
{
    DrawRectangle(chartX, chartY, chartWidth, chartHeight, Color{ 240, 240, 240, 255 });
    DrawRectangleLines(chartX, chartY, chartWidth, chartHeight, GRAY);

    if (barValues.empty()) return;

    // Scale bar relative to largest value so it can fit
    int maxValue = *std::max_element(barValues.begin(), barValues.end());
    if (maxValue <= 0) maxValue = 1;

    float gap = 2.0f;
    float barWidth = (chartWidth - gap * ((float)barValues.size() + 1.0f)) / (float)barValues.size();
    if (barWidth < 1.0f) barWidth = 1.0f;

    for (int index = 0; index < barValues.size(); index++)
    {
        float normalized = (float)barValues[index] / (float)maxValue;
        float barHeight = normalized * (chartHeight - 8.0f);
        float barX = chartX + gap + (barWidth + gap) * index;
        float barY = chartY + chartHeight - barHeight;

        //Change colour for readability
        Color barColor = BLUE;
        if (barStates[index] == ComparingBar) barColor = ORANGE;
        if (barStates[index] == SwappingBar) barColor = RED;
        if (barStates[index] == SortedBar) barColor = GREEN;

        DrawRectangle(barX, barY, barWidth, barHeight, barColor);
    }
}

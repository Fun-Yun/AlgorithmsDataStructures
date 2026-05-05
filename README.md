# Sorting/Maze Visualizer Assignment

A C++ [raylib](https://www.raylib.com/) algorithms visualizer built

---

`L` - Switch mode (Sorting / Maze)  
`R` - Reset current mode scene  
`Up / Down` - Change speed  

Sorting mode:
- `1` Bubble Sort
- `2` Selection Sort
- `3` Heap Sort

Maze mode:
- `1` BFS
- `2` DFS

## Features

- Sorting visualizer with 3 algorithms (Bubble, Selection, Heap)
- Live comparison/swap counters for sorting
- Maze visualizer with 2 pathfinding algorithms (BFS, DFS)
- 25x25 randomized solvable maze with walls
- Shared maze layout between BFS and DFS (fair comparison)
- Mouse-to-cheese animation when a path is found
- Single app with mode switch (`L`) to view both assignments

## Design Approach

Because I am still learning C++, I designed this project around small, readable classes with clear responsibilities:

- **Scene-style algorithm classes** – each algorithm has its own `UpdateScene()` and `DrawScene()` flow.
- **Manager classes** – one manager for maze scenes and one manager for sorting scenes, each handling scene selection, update, draw, reset, and speed.
- **Shared game loop** – input -> update -> draw in `main.cpp`, with mode-based routing.
- **Simple visualization states** – arrays/cells keep lightweight color/state flags that are easy to reason about while stepping through algorithms.

---

## Project Breakdown

### Entry Point & Loop

- **`main.cpp`**
  - Initializes the raylib window (`1280x720`) and sets FPS to 60.
  - Creates:
    - `MazeSceneManager` for maze scenes (BFS/DFS)
    - `SortSceneManager` for sorting scenes (Bubble/Selection/Heap)
  - Routes input/update/draw based on active mode (`isMazeMode`), toggled by `L`.

### Sorting Visualizer

- **`SortScene` (base class)**
  - Shared data:
    - `SortableBars bars`
    - `outerIndex`, `innerIndex`
    - `sorted`, `comparisons`, `swaps`
  - Shared HUD drawing helpers:
    - `DrawTopHud(...)`
    - `DrawBottomHud(...)`

- **Concrete sorting scenes**
  - **`BubbleSortScene`** – pairwise swaps with early-exit if no swaps in a pass.
  - **`SelectionSortScene`** – scans for minimum and swaps into place.
  - **`HeapSortScene`** – phased heap build + extraction with sift-down logic.

- **`SortableBars`**
  - Stores bar values and bar states.
  - Handles randomization (`Shuffle`), swaps, state flags, and drawing.
  - Uses colors to show comparing/swapping/sorted bars.

- **`SortSceneManager`**
  - Holds all sorting scenes.
  - Handles speed via steps-per-frame.
  - Handles scene selection and reset for sorting mode.

### Maze Visualizer

- **`MazeScene` (base class)**
  - Shared grid data:
    - `rows`, `cols` (25x25)
    - start/goal indices
    - `gridCellStates`, `cameFrom`, `visitedCells`
    - `path`, counters, solved/failed flags
  - Shared helpers:
    - grid index conversion (`ToIndex`, `ToRowCol`)
    - neighbor collection
    - path reconstruction (`RebuildPath`)
    - mouse animation (`UpdateMouseAnimation`)
    - shared HUD draw helpers (`DrawTopHud`, `DrawBottomHud`)

- **Maze generation (`BuildSolvableMaze`)**
  - Uses randomized DFS backtracking carve on a wall-filled grid.
  - Adds some random extra openings.
  - Guarantees start and goal are open.

- **`BFSVisualizer`**
  - Uses queue-like frontier (`std::deque`).
  - Marks visitedCells on enqueue.
  - Good for shortest path in unweighted grid.

- **`DFSVisualizer`**
  - Uses stack-like frontier (`std::vector` push/pop back).
  - Checks visitedCells on pop.
  - Usually finds a valid path quickly, but not guaranteed shortest.

- **`MazeSceneManager`**
  - Holds BFS and DFS scenes.
  - On reset:
    - regenerates one maze layout
    - applies same layout to both scenes
  - Ensures fair BFS vs DFS comparison on identical map.

---

## Design Patterns in This Project

### 1. State-Like Scene Pattern

**What it is:** One active algorithm scene updates and draws per frame.

**Where it’s used:**
- `MazeSceneManager` (maze scenes)
- `SortSceneManager` (sorting scenes)
- Scene classes (`BFSVisualizer`, `DFSVisualizer`, `BubbleSortScene`, etc.)

---

### 2. Strategy-by-Implementation (Algorithm Swapping)

**What it is:** Same interface shape (`Update/Draw/Reset/Name`) with different internal algorithm logic.

**Where it’s used:**
- Sorting scene subclasses
- Maze scene subclasses

---

### 3. Shared Data + Visual State

**What it is:** Keep algorithm data structures and visual state tightly coupled so every step can be seen immediately.

**Where it’s used:**
- `SortableBars` with `BarState`
- `MazeScene` with `MazeCellState`

---

## Summary Table

| Area | Implemented |
|---|---|
| Sorting algorithms | Bubble, Selection, Heap |
| Pathfinding algorithms | BFS, DFS |
| Maze size | 25 x 25 |
| Solvable maze | Yes |
| Shared maze for BFS/DFS | Yes |
| Counters | Sorting comparisons/swaps, maze visitedCells/path length |
| Bonus animation | Mouse follows path to cheese |

---

## File Layout (High Level)

- `main.cpp` – app loop, mode switch, input routing.
- `BarArray.h/.cpp` – sortable bars and draw logic.
- `SortScene.h/.cpp` – sorting base and 3 sorting algorithms.
- `SortingManager.h/.cpp` – sorting scene manager.
- `MazeScene.h/.cpp` – maze base + BFS + DFS + generation + drawing.
- `SceneManager.h/.cpp` – maze scene manager and shared layout reset logic.

---

## Reflection



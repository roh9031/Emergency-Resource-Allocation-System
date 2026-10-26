# core/

All algorithms and data types. This is the heart of the project and the part graded for the algorithms course.

## Rules

- Pure C++17 and the standard library only. No external libraries here.
- Algorithms take data as parameters and return results. **No `cin`, `cout`, or file access inside algorithm functions.** (File reading lives only in `io.cpp`.)
- Object oriented
- Every function gets a comment with its time and space complexity.

## Files to create

| File | Contents | Owner | Iteration |
|------|----------|-------|-----------|
| `types.h` | `Site`, `Depot`, `Edge`, `Allocation`, `Metrics`, `Params`, `Scenario` (started) | both | 1 |
| `io.h/.cpp` | `loadSites`, `loadDepots`, `loadRoadGraph`, `saveResults` with validation | B | 1 |
| `search_sort.h/.cpp` | `mergeSortSites`, `binarySearchSite`, `buildSpatialGrid`, `findNearestNode` | B | 1–2 |
| `graph.h/.cpp` | `dijkstra`, `buildTravelMatrix`, `closeRoad` | B | 2 |
| `greedy.h/.cpp` | `greedyAllocate` (A1) | B | 2 |
| `flow.h/.cpp` | `buildFlowNetwork`, `addEdge`, `edmondsKarp` (A2), `bellmanFord`, `minCostFlowAllocate` (A3, A4) | A | 2–3 |
| `fair.h/.cpp` | `maxMinFairAllocate` (A5) | A | 3 |
| `metrics.h/.cpp` | `validateAllocation`, `computeMetrics`, `bruteForceOptimal` | A + B | 3 |
| `generator.h/.cpp` | `generateInstance` (seeded) | B | 4 |
| `experiment.h/.cpp` | `runExperiment` | B | 4 |


## Tie-breaking 

- Greedy site order: urgency desc, then demand desc, then ID asc (stable merge sort).
- Greedy depot order per site: travel time asc, then ID asc.

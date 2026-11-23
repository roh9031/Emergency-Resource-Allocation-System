# core/

All algorithms and data types. This is the heart of the project and the part graded for the algorithms course.

## Rules

- Pure C++17 and the standard library only. No external libraries here.
- Object oriented: one class per `.h`/`.cpp` pair, private data, constructors validate their invariants. Class templates are header-only.
- Algorithms take data as parameters and return results. **No `cin`, `cout`, or file access outside `io/`.**
- No library sorting or searching inside graded algorithm code (we write `MergeSort` and `BinarySearch` ourselves).
- Every algorithmic method gets a comment with its time and space complexity.
- Errors: bad values throw `std::invalid_argument`, bad indices throw `std::out_of_range`.

## Conventions

- Naming: classes `PascalCase`, methods `camelCase`, private members end in `_`.
- Inside algorithms, depots are indexed `0..m-1` and sites `0..n-1` in the order `Scenario` stores them. Real IDs from the CSV files only appear in results.
- Units: travel time in minutes (`double`), quantities in kits (`int`). An unreachable pair has travel time `std::numeric_limits<double>::infinity()`.
- Flow costs are integers (`long long`). A `double` that becomes a cost is multiplied by `kCostScale` (`algorithms/CostScale.h`) and rounded with `std::llround`.

## Layout

| Folder | Responsibility | Classes | Owner |
|--------|----------------|---------|-------|
| `domain/` | Problem data with enforced invariants | `Site`, `Depot`, `Params`, `RoadGraph`, `Scenario` | Catalina |
| | | `SpatialGrid`, `Allocation`, `AllocationResult` | Rohini |
| `algorithms/` | All graded algorithms | `MergeSort` (+ `UrgencyOrder`), `GreedyAllocator` (A1), `LexicographicFlowAllocator` (A3), `MinCostFlowAllocator` (A4), `FlowNetwork::bellmanFord` / `augment` | Catalina |
| | | `BinarySearch`, `Dijkstra`, `FlowNetwork` (residual graph, `maxFlow`), `AllocationStrategy`, `FlowBasedAllocator`, `EdmondsKarpAllocator` (A2), `MaxMinFairAllocator` (A5), `AllocatorFactory`, `CostScale` | Rohini |
| `evaluation/` | Checking, measuring, experiments | `Metrics`, `MetricsCalculator` | Catalina |
| | | `AllocationValidator`, `BruteForceSolver`, `InstanceGenerator`, `ExperimentRunner`, `Timer` | Rohini |
| `io/` | Files in and out, with validation | `CsvReader`, `DimacsGraphReader`, `ScenarioLoader`, `ResultWriter` | Catalina |
| `service/` | Facade used by the CLI, the server and the tests | `AllocationService` | Rohini |

## Tie-breaking

- Greedy site order: urgency desc, then demand desc, then ID asc (stable merge sort).
- Greedy depot order per site: travel time asc, then ID asc.

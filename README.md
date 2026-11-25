# ReliefRoute

**When is greedy good enough?** A web-based emergency resource allocation system with a C++ engine that compares greedy, maximum-flow, lexicographic-flow, min-cost-flow, and max-min fair allocation under scarcity.

> Course project for `Fundamentals Of Computer Engineering`. Team: `Rohini Bidnurmath`, `Catalina Buitano`.

**Intended users:** emergency planners who must split a limited supply of relief kits from several depots across affected sites, and who want to know when a simple, explainable rule (most urgent site first) is close enough to the best possible allocation.

## Research question

For a capacitated single-commodity allocation model, under what combinations of supply scarcity, network reachability, and urgency distribution do transparent greedy policies achieve near-optimal urgency-weighted delivery, and when do they incur substantial losses relative to an exact network-flow benchmark?

The full proposal is in [`docs/proposal/`](docs/proposal/).

## Methods

| ID | Method | Role |
|----|--------|------|
| A1 | Urgency-first greedy | Heuristic being studied |
| A2 | Edmonds–Karp max flow | Maximum volume, arbitrary tie-breaking |
| A3 | Lexicographic flow | Max volume, then max urgency |
| A4 | Urgency-weighted min-cost flow | Exact optimum (main benchmark) |
| A5 | Max-min fair allocation | Equity benchmark |

Supporting algorithms (all written by us): merge sort, binary search, grid nearest-node search, BFS, Dijkstra, Bellman–Ford.

## Current status (iteration 3 prototype)

The prototype runs end to end: **read three CSV files → validate them → allocate with A1 (urgency-first greedy), A2 (Edmonds–Karp maximum flow) or both → print the result**. Bad input lines are reported and skipped instead of crashing the program.

A2 is the core operation. The second approach, **A1 urgency-first greedy** (`LinkGreedyAllocator`, built on our own merge sort and binary search), runs over the same input, and the menu can compare the two side by side. On the samples they already show the trade-off the project studies:

| Sample | A1 kits delivered | A2 kits delivered | A1 urgency-weighted | A2 urgency-weighted |
|--------|------------------:|------------------:|--------------------:|--------------------:|
| `basic/` | 18 | 18 | 172 | 172 |
| `motivating/` | 1 | 2 | 10 | 19 |
| `invalid/` | 17 | 17 | 113 | 107 |
| `large/` | 3982 | 3990 | 23894.1 | 20071.5 |

Urgency-weighted delivery is the sum of kits × site urgency. A2 delivers the most kits but ignores urgency, so on `large/` greedy delivers 8 fewer kits but about 19% more urgency-weighted value.

Until travel times exist (iteration 4), the allowed depot → site connections come from a third file, `links.csv`. See [`data/README.md`](data/README.md).

## Build

Requirements: a C++17 compiler (GCC 9+ or Clang 10+) and CMake 3.16+. No other libraries are needed. On Windows we use [MSYS2](https://www.msys2.org/) UCRT64 (`g++`, `cmake`, `ninja`).

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

**Windows, Git Bash:** if the tests or the program exit immediately with code `0xc0000139`, Git's own older `libstdc++-6.dll` is being found before the compiler's. Put the compiler's folder first on `PATH`, once per terminal or in `~/.bashrc`:

```bash
export PATH="/c/msys64/ucrt64/bin:$PATH"
```

## Run

Run from the repository root, so the default sample folder is found.

**Interactive menu:**

```bash
./build/reliefroute-cli
```

| Option | What it does |
|--------|--------------|
| 1. Load a sample folder | asks for a folder; Enter loads `data/sample/basic` |
| 2. Show load problems | every skipped line as `<file>:<line>: <message>` |
| 3. Run allocation | asks for a method: 1. A1 greedy, 2. A2 max flow (each prints the shipment table and per-site fill rates), or 3. Compare A1 vs A2 (kits delivered, urgency-weighted value and runtime per method) |
| 4. Exit | |

Bad input (letters, decimals, numbers out of range) is asked again, both in the menu and at the method prompt. A missing folder prints an error and the menu continues.

**One shot:** load a folder, list its problems, run both methods, print both tables and the comparison, exit:

```bash
./build/reliefroute-cli data/sample/motivating
```

Example output (shortened):

```
Allocation (A1)
Depot        ->  Site          Units
------------------------------------
Depot A (1)  ->  Site X (1)        1
...
Allocation (A2)
Depot        ->  Site          Units
------------------------------------
Depot A (1)  ->  Site Y (2)        1
Depot B (2)  ->  Site X (1)        1
...
Comparison
Method                       Kits   Weighted value   Runtime (ms)
-----------------------------------------------------------------
A1 greedy (urgency first)       1            10.00          0.015
A2 max flow                     2            19.00          0.015
```

Greedy serves the most urgent site X first from depot A, so Y (reachable only from A) gets nothing and only 1 kit is delivered. Max flow finds the allocation that delivers both. Runtimes vary from run to run.

## Sample data

Four scenarios in [`data/sample/`](data/sample/), each with `sites.csv`, `depots.csv` and `links.csv`:

| Folder | What it tests | Expected max flow |
|--------|---------------|-------------------|
| `basic/` | normal case: 2 depots compete for 3 sites | 18 |
| `motivating/` | the case greedy gets wrong | 2 (exactly A→Y, B→X) |
| `invalid/` | 7 deliberate bad lines mixed with valid ones | 17 (from the valid lines) |
| `large/` | 200 sites, 20 depots, 400 links | 3990 (= total supply) |

[`data/sample/README.md`](data/sample/README.md) explains how each expected value was derived and lists every bad line in `invalid/`. `large/` is generated by `python scripts/make_sample_large.py` (fixed seed, standard library only); the generated files are committed, so Python is not needed to build or test.

## Tests

```bash
ctest --test-dir build --output-on-failure
```

One test executable (`unit-tests`) runs every test file in [`tests/`](tests/) with a small assert-style harness (`tests/test_harness.h`, no external framework). Each test file covers one class, with normal, boundary, invalid, empty and large cases. The allocator tests check the expected values above, both from hand-built data and through the real loader.

## Progress

| Feature | Status | Evidence or remaining work |
|---------|--------|----------------------------|
| Sample input files | Complete | `data/sample/`: four scenarios with derived expected results |
| Load input data | Complete | `CsvReader`, `SampleLoader`: checks field count, numbers, negatives, duplicate IDs, unknown links; reports `<file>:<line>` |
| Domain classes | Complete | `Site`, `Depot`, `Params` with validated constructors |
| Residual flow network + Edmonds–Karp | Complete | `FlowNetwork::maxFlow`, 21 unit tests |
| A2 allocation (core operation) | Complete | `LinkMaxFlowAllocator`: 18 / 2 / 17 / 3990 on the four samples |
| Results and timing | Complete | `AllocationResult`, `Timer` |
| Command-line menu | Complete | `cli/`: menu with method choice and A1 vs A2 comparison, one-shot mode, table and fill-rate summary |
| Binary search | Complete | `BinarySearch` template, used by the greedy allocator |
| Merge sort | Complete | `MergeSort` template (stable) and `UrgencyOrder`, 7 unit tests |
| A1 greedy (second approach) | Complete over `links.csv` | `LinkGreedyAllocator`, 9 unit tests, in the menu; remaining (iteration 4): depot order by travel time |
| Road network and Dijkstra travel times | Planned (iteration 4) | replaces `links.csv`: a link exists when travel time ≤ deadline |
| A3–A5, comparison experiments | Planned (iteration 4) | min-cost flow (Bellman–Ford), lexicographic and max-min fair flow |
| Allocation validator, brute-force checker | Planned (iteration 4) | independent check of every method's output |
| REST server and web map | Planned | after the C++ engine is complete |

## Known limitations

- **Links are given, not computed.** Which depot can serve which site comes from `links.csv`; real road travel times arrive in iteration 4.
- **Only two methods so far.** A2 maximises the total number of kits delivered but ignores urgency, so when several maximum allocations exist it may pick any of them. A1 follows urgency but never undoes a shipment. Neither is the urgency-weighted optimum; that benchmark (A4) arrives in iteration 4.
- **A1 tries depots in ID order.** Without travel times, each site's depots are tried lowest ID first; in iteration 4 the order becomes nearest depot first.
- **Coordinates are not used yet.** `lat` and `lon` are read and stored for the road network later.
- **A2 looks up sites and depots by linear scan**, which is O(n) per link. That is fine for these sizes. A1 already uses sort once + binary search, and A2 will share that in iteration 4.
- **Menu and one-shot mode only.** There is no batch mode for experiments, no server and no web interface yet.

## Repository layout

| Folder | What goes there | Owner |
|--------|-----------------|-------|
| `core/` | All algorithms and data types (pure C++, no I/O in algorithms) | A + B |
| `server/` | C++ REST API over the core | A |
| `cli/` | C++ menu and batch mode over the core | B |
| `web/` | Browser frontend (display only, no algorithms) | B |
| `tests/` | Unit, API, and consistency tests | both |
| `data/` | Input data (raw downloads are not committed) | A |
| `scripts/` | Data download and preprocessing (Python) | A |
| `experiments/` | Experiment configs and results | B |
| `docs/` | Proposal, API reference, design notes, report | both |
| `third_party/` | Header-only libraries (httplib, json) | A |

Every folder has its own README with a to-do list.

## Contributing

Read [`CONTRIBUTING.md`](CONTRIBUTING.md).

# ReliefRoute

**When is greedy good enough?** A web-based emergency resource allocation system with a C++ engine that compares greedy, maximum-flow, lexicographic-flow, min-cost-flow, and max-min fair allocation under scarcity.

> Course project for `Fundamentals Of Computer Engineering`. Team: `Rohini Bidnurmath`, `Catalina Buitano`.

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

## Repository layout

| Folder | What goes there | Owner |
|--------|-----------------|-------|
| `core/` | All algorithms and data types (pure C++, no I/O in algorithms) | A + B |
| `server/` | C++ REST API over the core | A |
| `cli/` | C++ menu and batch mode over the core | B |
| `web/` | Browser frontend (display only, no algorithms) | B |
| `tests/` | Unit, API, and consistency tests | both |
| `data/` | Input data (raw downloads are not committed) | B |
| `scripts/` | Data download and preprocessing (Python) | B |
| `experiments/` | Experiment configs and results | B |
| `docs/` | Proposal, API reference, design notes, report | both |
| `third_party/` | Header-only libraries (httplib, json) | A |

Every folder has its own README with a to-do list.


## Contributing

Read [`CONTRIBUTING.md`](CONTRIBUTING.md) 

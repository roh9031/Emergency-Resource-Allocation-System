# tests/

Run with `ctest --test-dir build --output-on-failure`. CI runs them on every push and PR.

## Structure

- `test_main.cpp` — tiny assert-based test runner (no external framework).
- Add one file per core module later (`test_search_sort.cpp`, `test_flow.cpp`, …) and register it in `CMakeLists.txt`.
- `api/run_api_tests.sh` — curl-based API tests (create in iteration 2).

## Must-have tests (from the proposal's testing plan)

- [ ] Hand-solved 2-depot, 3-site instance
- [ ] Motivating example: A1 gives W = 10, A4 gives W* = 19
- [ ] A4 equals `bruteForceOptimal()` on 200 random tiny instances
- [ ] A3 volume = A2 volume; A4 weighted value ≥ every other method
- [ ] Zero supply; unreachable site; one depot one site; equal urgencies
- [ ] Merge sort: empty, single, duplicates, reverse-sorted (and stable)
- [ ] Binary search: first, last, missing
- [ ] Invalid CSV lines, missing files, negative values, duplicate IDs
- [ ] Closing a road never increases the A4 optimum
- [ ] CLI and `/api/compare` give identical results for the same scenario
- [ ] API: malformed JSON → 400, unknown scenario → 404

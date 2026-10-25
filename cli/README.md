# cli/

Command-line interface over `core/`. 

## Two modes

- **Menu:** `./build/reliefroute-cli` — interactive text menu (see proposal, Algorithm 1).
- **Batch:** `./build/reliefroute-cli --batch experiments/configs/grid.cfg --out experiments/results/`

## To do

- [ ] Iteration 1: menu loop, `getValidatedInt` (re-prompt on bad input), load files, search by ID, list by urgency
- [ ] Iteration 2: run A1/A2, formatted allocation table
- [ ] Iteration 3: run A3–A5, comparison table
- [ ] Iteration 4: batch mode with command-line arguments

Only UI code lives here. Anything algorithmic belongs in `core/`.

# experiments/

## Design (from the proposal)

| Factor | Levels |
|--------|--------|
| Scarcity ρ = total supply / total demand | 0.3, 0.5, 0.7, 0.9, 1.1 |
| Reachability (share of eligible depot–site pairs) | 0.1, 0.25, 0.5, 1.0 |
| Urgency skew (share of critical sites) | 10%, 30%, 50% |
| Sites n | 50, 200, 1000, 5000 |
| Seeds per setting | 30 |

## Folders

- `configs/` — grid configuration files read by `reliefroute-cli --batch`.
- `results/` — raw output CSVs (gitignored). Commit only small summary CSVs and final figures under `docs/figures/`.

## Intial plan

- Debug on n = 50 and 200 first; full small sweeps take minutes.
- Run n = 1000 and 5000 overnight, splitting seeds across CPU cores.
- Fixed seeds mean instances never need to be stored; regenerate from the seed.

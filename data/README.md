# data/

| Folder | Contents | Committed? |
|--------|----------|------------|
| `raw/` | Original downloads (large) | No (gitignored) |
| `processed/` | Files produced by the Python scripts in `scripts/` | No (regenerate with scripts) |
| `sample/` | Tiny hand-made scenarios for testing and demos | Yes |

## Case study

One past major hurricane disaster declaration in **Florida** (`FL`). Real data is only needed for the iteration-4 case study; the main experiments use synthetic scenarios from `InstanceGenerator`.

## Sources (downloaded into `raw/`)

| Dataset | Used for |
|---------|----------|
| OpenFEMA Disaster Declarations Summaries (v2) | Affected counties (sites) |
| Census county population estimates (CO-EST2025-alldata) | Demand |
| CDC/ATSDR Social Vulnerability Index 2022 (county) | Urgency weights |
| Census Gazetteer counties file (2026) | County coordinates |
| HIFLD hospitals (DataLumos archive) | Depot locations |
| OpenStreetMap, downloaded with the Python library OSMnx | Road network and travel times |

Exact URLs, versions, access dates, and licenses for every file are in `SOURCES.md`.

**Road network:** the DIMACS 9th Challenge server was unreachable, so the road graph is built from OpenStreetMap with OSMnx (current roads, real speed limits and travel times) and exported in the **DIMACS `.gr`/`.co` format**. The C++ reader is the same either way. Map data © OpenStreetMap contributors.

## Derived values (assumptions)

- Demand = population × kits per resident × affected fraction.
- Depot supply: assumed (e.g., proportional to hospital beds).
- Travel time: from OSMnx per road segment, based on speed limits, stored in whole seconds (at least 1).

## Processed files (`processed/`)

| File | Format |
|------|--------|
| `sites.csv` | `id,name,demand,urgency,lat,lon`. `id` = 5-digit county FIPS as an integer; `urgency` = SVI score (`double`, > 0) |
| `depots.csv` | `id,name,supply,lat,lon`. `id` = 1, 2, 3 …; `supply` from the documented assumption |
| `roads.gr` | DIMACS arcs `a u v w`, 1-based nodes, weight `w` = travel time in whole seconds (≥ 1) |
| `roads.co` | DIMACS coordinates `v id lon lat`, integers with 6 implied decimals (longitude first) |

## Scripts (`scripts/`, iteration 4)

- `build_case_study.py`: joins FEMA + Census + SVI + Gazetteer on county FIPS, plus HIFLD hospitals → `sites.csv`, `depots.csv`.
- `build_road_network.py`: OSMnx download for the affected counties → `roads.gr`, `roads.co`.
- `plot_results.py`: figures from the experiment CSVs.

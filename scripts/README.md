# scripts/

Python helpers for data preparation and plotting only. **No allocation algorithms here** those are in C++.

## Scripts to write

- `download_data.py` — fetch raw datasets into `data/raw/` and log URLs and dates.
- `build_case_study.py` — join FEMA counties + Census population + SVI on county FIPS; attach depot locations; write `data/processed/sites.csv` and `depots.csv`.
- `plot_results.py` — read `experiments/results/*.csv`; produce gap heat maps, runtime-vs-size plots, and equity charts for the report.

Setup:
```bash
python3 -m venv .venv && source .venv/bin/activate
pip install pandas matplotlib requests
```

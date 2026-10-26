# data/

| Folder | Contents | Committed? |
|--------|----------|------------|
| `raw/` | Original downloads (large) | No (gitignored) |
| `processed/` | Cleaned CSVs produced by `scripts/` | No (regenerate with scripts) |
| `sample/` | Tiny hand-made scenarios for testing and demos | Yes |

## Sources to download (into `raw/`)

| Dataset | Used for |
|---------|----------|
| OpenFEMA Disaster Declarations Summaries (v2) | Affected counties (sites) |
| Census county population estimates (CO-EST2025-alldata) | Demand |
| CDC/ATSDR Social Vulnerability Index (county) | Urgency weights |
| HIFLD Open hospitals, fire/EMS stations (DataLumos archive) | Depot locations |
| DIMACS 9th Challenge road networks (TIGER `.gr` / `.co`) | Travel times |

Record the exact download URL and date for every file in `data/SOURCES.md` (create it), so results are reproducible. Download the SVI and HIFLD files early; their hosting has changed before.

## Derived values (Assumptions)

- Demand = population × kits per resident × affected fraction.
- Depot supply: assumed (e.g., proportional to hospital beds).
- Travel time = road length / speed by road class.

## CSV formats

```
sites.csv    id,name,demand,urgency,lat,lon
depots.csv   id,name,supply,lat,lon
```

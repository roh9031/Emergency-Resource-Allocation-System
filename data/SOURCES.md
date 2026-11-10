# Data sources

Every external dataset used by ReliefRoute, with enough detail to download it
again and cite it in the report. Raw downloads live in `data/raw/` and are
**not committed** (see `data/README.md`); cleaned files produced by `scripts/`
go in `data/processed/`. Fields marked TODO still need to be filled in.

---

## 1. OpenFEMA Disaster Declarations Summaries (v2)

- **Title:** OpenFEMA Disaster Declarations Summaries (v2)
- **Publisher:** FEMA (OpenFEMA)
- **Page URL:** https://www.fema.gov/openfema-data-page/disaster-declarations-summaries-v2
- **File(s) downloaded:** the full CSV of Disaster Declarations Summaries v2 (about 70,600 records). File name: `DisasterDeclarationsSummaries.csv`
- **Version / vintage:** v2; last data refresh 11-10-2026 (as shown on page)
- **Date accessed:** 11/09/2026
- **License / terms:** OpenFEMA Terms and Conditions page (TODO: URL and summary)
- **Citation for the report:** TODO: copy the citation format from the OpenFEMA Terms and Conditions page
- **Notes:**
  - Used for: affected counties (sites).
  - Columns we'll use: `declarationType`, `state`, `declarationDate`, `incidentType`, `femaDeclarationString`, `fipsStateCode`, `fipsCountyCode`, `designatedArea`.
  - Known quirks:
    - Keep `declarationType` DR only.
    - Drop `fipsCountyCode` 000 (statewide declarations).
    - One county appears several times per disaster.
    - Keep FIPS codes as text.

---

## 2. U.S. Census Bureau county population estimates

- **Title:** TODO
- **Publisher:** U.S. Census Bureau
- **Page URL:** https://www.census.gov/data/tables/time-series/demo/popest/2020s-counties-total.html
- **File(s) downloaded:**
  - `co-est2025-alldata.csv` (data)
  - `CO-EST2025-ALLDATA.pdf` (file layout)
- **Version / vintage:** "Vintage 2025, released March 2026."
- **Date accessed:** 11/09/2026
- **License / terms:** TODO
- **Citation for the report:** TODO
- **Notes:**
  - Used for: demand (population × kits per resident × affected fraction).
  - Columns we'll use: `SUMLEV` (keep 050), `STATE`, `COUNTY`, `STNAME`, `CTYNAME`, `POPESTIMATE2025`.
  - Known quirks: may need encoding latin-1.

---

## 3. CDC/ATSDR Social Vulnerability Index (SVI), county level

- **Title:** TODO
- **Publisher:** CDC/ATSDR
- **Page URL:** https://www.atsdr.cdc.gov/place-health/php/svi/svi-data-documentation-download.html
- **File(s) downloaded:**
  - CSV: Year 2022, Geography United States, Geography Type Counties. File name: `SVI_2022_US_COUNTY.csv`
  - 2022 documentation PDF (has the column dictionary). File name: `SVI2022Documentation_ZCTA.pdf`
- **Version / vintage:** 2022
- **Date accessed:** 11/09/2026
- **License / terms:** TODO
- **Citation for the report:** TODO (the proposal cites Flanagan et al. 2011 [8] for the SVI method; the dataset itself still needs a citation)
- **Notes:**
  - Used for: urgency weights.
  - Columns we'll use: TODO (see the column dictionary in the 2022 documentation PDF)
  - Known quirks:
    - Don't open in Excel: it drops leading zeros in FIPS codes.
    - Hosting has changed before, so download early.

---

## 4. HIFLD OPEN Hospitals (DataLumos archive)

- **Title:** HIFLD OPEN Hospitals
- **Publisher:** Oak Ridge National Laboratory, and United States Department of Homeland Security; distributed by Inter-university Consortium for Political and Social Research (ICPSR), Ann Arbor, MI
- **Page URL:** https://www.datalumos.org/datalumos/project/239108/version/V1/view
- **File(s) downloaded:**
  - `hospitals-3-geojson.zip` (GeoJSON)
  - `metadata.xml`
- **Version / vintage:** V1, distributed 2025-10-22
- **Date accessed:** 11/09/2026
- **License / terms:** TODO
- **Citation for the report:**
  Oak Ridge National Laboratory, and United States Department of Homeland Security. HIFLD OPEN Hospitals. Ann Arbor, MI: Inter-university Consortium for Political and Social Research [distributor], 2025-10-22. https://doi.org/10.3886/E239108V1
- **Notes:**
  - Used for: depot locations; supply is assumed (e.g. proportional to hospital beds).
  - Columns we'll use: TODO (coordinates, bed counts; exact field names TODO)
  - Known quirks:
    - The original HIFLD Open site was deactivated on August 26, 2025, which is why we use the archive.
    - GeoJSON coordinates are [longitude, latitude].
    - Check bed counts for placeholder values.
    - Hosting has changed before, so download early.
  - Fire/EMS stations (listed in `data/README.md`): TODO

---

## 5. Road network

- **Title:** DIMACS 9th Challenge, `USA-road-d.FLA` (planned); replacement built from OpenStreetMap with OSMnx
- **Publisher:** TODO
- **Page URL:** TODO
- **File(s) downloaded:** none. The DIMACS download is blocked from our network (the server returns a 236-byte error page; tried http and https on 11/09/2026).
- **Version / vintage:** TODO
- **Date accessed:** tried 11/09/2026 (DIMACS)
- **License / terms:** OpenStreetMap data is under the Open Database License (ODbL). Attribution: © OpenStreetMap contributors.
- **Citation for the report:** TODO
- **Notes:**
  - Plan: build the road network from OpenStreetMap with OSMnx in iteration 4, and export it to DIMACS `.gr`/`.co` format, so the C++ reader still takes DIMACS files.
  - Used for: travel times via Dijkstra.
  - Region extracted: TODO
  - OSMnx version and query parameters: TODO
  - Known quirks: TODO

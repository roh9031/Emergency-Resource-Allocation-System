"""Generate data/sample/large/: 200 sites, 20 depots, a fixed random seed

Output (written next to the other sample scenarios):
    data/sample/large/sites.csv    id,name,demand,urgency,lat,lon
    data/sample/large/depots.csv   id,name,supply,lat,lon
    data/sample/large/links.csv    depotId,siteId,capacity

Usage (from the repository root):
    python scripts/make_sample_large.py

The generated CSVs are committed so tests never need to run this script
With the same seed and the same Python 3 version the output is identical

Why the expected max flow is known without running a flow algorithm

Construction:
  1. The 200 sites are split into 20 DISJOINT groups of 10. Group j holds
     the "own" sites of depot j
  2. Each depot j gets supply_j between 50% and 90% of the total demand of
     its own group so supply_j <= ownDemand_j for every j
  3. Depot j links to all of its own sites plus EXTRA_LINKS random sites of
     other groups every link has capacity min(supply_j, demand_i)

Claim: max flow = total supply.
  * Upper bound: all flow leaves the source through the source -> depot
    arcs whose capacities sum to total supply no flow can exceed that cut
  * Achievable: let every depot j fill its own sites one by one sending
    min(remaining supply, demand_i) to each because ownDemand_j >= supply_j,
    depot j runs out of supply before it runs out of own sites so it ships
    its whole supply on each link the amount sent is <= demand_i and
    <= supply_j so it fits within min(supply_j, demand_i) the groups are
    disjoint so no site receives from two depots and no demand is exceeded
  * The extra cross group links only add routes adding arcs can never
    lower a maximum flow and the upper bound above still holds
Hence max flow = total supply exactly Total supply < total demand as well
so the sink side is never the limit

No allocation algorithm runs here (see scripts/README.md); the assertions
below only check the conditions the argument relies on
"""

import csv
import random
from pathlib import Path

SEED = 42
NUM_SITES = 200
NUM_DEPOTS = 20
SITES_PER_DEPOT = NUM_SITES // NUM_DEPOTS  # 10 own sites per depot
EXTRA_LINKS = 10                          # cross group links per depot
DEMAND_MIN, DEMAND_MAX = 5, 50
SUPPLY_FRACTION_MIN, SUPPLY_FRACTION_MAX = 0.5, 0.9
# placeholder coordinates around the other sample scenarios nothing uses
# them until travel times exist
LAT_MIN, LAT_MAX = 35.0, 36.0
LON_MIN, LON_MAX = -83.0, -82.0

OUT_DIR = Path(__file__).resolve().parent.parent / "data" / "sample" / "large"


def main():
    rng = random.Random(SEED)

    # sites ids 1 to 200 Site i belongs to the group of depot
    # (i - 1) // SITES_PER_DEPOT + 1 so groups are disjoint blocks of 10.
    sites = []
    for i in range(1, NUM_SITES + 1):
        sites.append({
            "id": i,
            "name": f"Site {i}",
            "demand": rng.randint(DEMAND_MIN, DEMAND_MAX),
            "urgency": round(rng.uniform(0.1, 10.0), 2),
            "lat": round(rng.uniform(LAT_MIN, LAT_MAX), 4),
            "lon": round(rng.uniform(LON_MIN, LON_MAX), 4),
        })
    demand = {s["id"]: s["demand"] for s in sites}

    depots = []
    links = []
    for j in range(1, NUM_DEPOTS + 1):
        own = list(range((j - 1) * SITES_PER_DEPOT + 1,
                         j * SITES_PER_DEPOT + 1))
        own_demand = sum(demand[i] for i in own)
        fraction = rng.uniform(SUPPLY_FRACTION_MIN, SUPPLY_FRACTION_MAX)
        supply = max(1, int(fraction * own_demand))  # int() rounds down
        assert supply <= own_demand, "supply_j must not exceed own demand"

        depots.append({
            "id": j,
            "name": f"Depot {j}",
            "supply": supply,
            "lat": round(rng.uniform(LAT_MIN, LAT_MAX), 4),
            "lon": round(rng.uniform(LON_MIN, LON_MAX), 4),
        })

        others = [i for i in range(1, NUM_SITES + 1) if i not in own]
        linked = own + rng.sample(others, EXTRA_LINKS)
        for i in linked:
            links.append({
                "depotId": j,
                "siteId": i,
                "capacity": min(supply, demand[i]),
            })

    total_supply = sum(d["supply"] for d in depots)
    total_demand = sum(demand.values())
    assert total_supply < total_demand, "total supply must be below demand"
    assert len({(l["depotId"], l["siteId"]) for l in links}) == len(links), \
        "no duplicate links"

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_csv(OUT_DIR / "sites.csv",
              ["id", "name", "demand", "urgency", "lat", "lon"], sites)
    write_csv(OUT_DIR / "depots.csv",
              ["id", "name", "supply", "lat", "lon"], depots)
    write_csv(OUT_DIR / "links.csv",
              ["depotId", "siteId", "capacity"], links)

    print(f"sites={len(sites)} depots={len(depots)} links={len(links)}")
    print(f"total demand={total_demand}")
    print(f"total supply={total_supply}  (= expected max flow)")


def write_csv(path, fields, rows):
    # newline="" stops the csv module writing \r\r\n on Windows
    with open(path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


if __name__ == "__main__":
    main()

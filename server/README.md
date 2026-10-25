# server/

C++ REST API over `core/`. Owner: A.

## Setup

1. Put `httplib.h` and `json.hpp` in `third_party/` (see that README).
2. Uncomment the server block in the root `CMakeLists.txt`.
3. Create `main.cpp` (start server, serve `web/` as static files) and `api.cpp` (route handlers).

## Endpoints (from the proposal)

| Method | Endpoint | Core function |
|--------|----------|---------------|
| GET | `/api/health` | — |
| GET | `/api/scenarios` | — |
| POST | `/api/scenarios` | `loadSites`, `loadDepots` |
| POST | `/api/scenarios/generate` | `generateInstance` |
| GET | `/api/scenarios/{id}/sites?sort=urgency` | `mergeSortSites` |
| GET | `/api/scenarios/{id}/sites/{siteId}` | `binarySearchSite` |
| POST | `/api/scenarios/{id}/roads/close` | `closeRoad`, `buildTravelMatrix` |
| POST | `/api/allocate` | A1–A5 |
| POST | `/api/compare` | A1–A5 |
| POST | `/api/experiments` | `runExperiment` |
| GET | `/api/experiments/{id}` | — |

## Rules

- Validate every request body. Bad input → `400` with a readable message. Unknown scenario → `404`.
- Handlers only parse, call `core/`, and serialize. No algorithm logic here.
- Document every endpoint (request and response JSON) in `docs/API.md`.

## To do

- [ ] Iteration 1: `/api/health`
- [ ] Iteration 2: scenarios + `/api/allocate`
- [ ] Iteration 3: `/api/compare`, road closures
- [ ] Iteration 4: experiments endpoints

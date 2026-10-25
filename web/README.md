# web/

Browser frontend. Owner: B. Served as static files by the C++ server.

## Rules

- Plain HTML, CSS, and JavaScript (no build step).
- **No algorithms in JavaScript.** The frontend only calls `/api/...` and displays results. This keeps all graded algorithm work in C++.
- Libraries: Leaflet (map, OpenStreetMap tiles) and Chart.js (charts), loaded from a CDN.

## Files to create

- `index.html` — layout: map, side panel, dashboard tab
- `app.js` — API calls, map drawing, charts
- `style.css`

## Screens

1. **Scenario map** — depots (squares, sized by supply), sites (circles, colored by urgency); click to inspect or edit; search by ID; toggle road closures.
2. **Run panel** — choose method(s) and parameters; draw deliveries as lines; shade sites by fill rate.
3. **Comparison dashboard** — metrics table and bar charts for A1–A5.
4. **Experiments page** — launch a grid; greedy-gap heat map.

## To do

- [ ] Iteration 2: map loads a scenario and draws one allocation
- [ ] Iteration 3: editor, comparison dashboard, road closures
- [ ] Iteration 4: experiments page, polish, screenshots for README

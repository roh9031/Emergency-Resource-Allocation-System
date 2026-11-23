# How we work

## Branches

- `main`: always builds and passes tests. Protected. Nobody pushes directly.
- `dev`: integration branch. All pull requests go here.
- `feature/<short-name>`: one branch per piece of work, e.g. `feature/merge-sort`, `feature/edmonds-karp`, `feature/web-map`.

At the end of each iteration, `dev` is merged into `main`.

## Everyday workflow (step by step)

```bash
# 1. Start from the latest dev
git checkout dev
git pull

# 2. Make a branch for your task
git checkout -b feature/merge-sort

# 3. Work, then commit small steps often
git add core/algorithms/MergeSort.h tests/test_merge_sort.cpp
git commit -m "Add merge sort for sites by urgency"

# 4. Push your branch
git push -u origin feature/merge-sort

# 5. On GitHub: open a Pull Request from feature/merge-sort into dev
```

If `dev` changed while you were working:

```bash
git checkout dev
git pull
git checkout feature/merge-sort
git merge dev        # fix any conflicts, then commit
git push
```

## Commit messages

Imperative mood, one idea per commit:

- Good: `Add BFS augmenting path to edmondsKarp`
- Good: `Fix off-by-one in binarySearchSite`
- Bad: `stuff`, `fixed it`, `update`

## Pull requests

- Target branch: `dev`.
- The other team member must approve before merging.
- CI must be green.
- Fill in the PR template (it appears automatically).
- Reviewer checks: correctness, tests included, clear names, comments where the logic is not obvious.

## Definition of done for any function

1. Pseudocode written first (in the PR description or `docs/`).
2. Implemented in `core/` with no `cin`/`cout` inside the algorithm.
3. Unit tests for normal, boundary, and invalid inputs.
4. Time and space complexity noted in a comment above the function.

## One-time GitHub setup (repo owner)

1. Create the repo, push this scaffold to `main`.
2. Create `dev`: `git checkout -b dev && git push -u origin dev`.
3. Settings → Branches → add protection rules for `main` and `dev`: require a pull request, 1 approval, and passing status checks.
4. Settings → Collaborators: add teammate (write) and instructor.
5. Settings → General → default branch: `dev`.

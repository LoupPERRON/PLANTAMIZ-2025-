# Copilot / AI Agent Guidance — PLANTAMIZ (Console Match-3)

This project is a console-adapted "match-3" game (inspired by FarmHeroes/CandyCrush). The codebase has multiple partial implementations and inconsistent naming/styles — follow these instructions when contributing or generating code.

Key files & high-level architecture
- `main.c` — process entry, seeds RNG and runs the game loop. Note: currently uses English symbols (`GameState`, `initGame`) that are not defined in headers.
- `plantamitz.h` — canonical header constants and primary game structure (French naming => `EtatJeu`, `NB_LIGNES`, `NB_COLONNES`). Use these macros/structs as the authoritative layout for board dimensions & state.
- `plantamiz.c` — mechanics (board generation, group detection, gravity, stabilization; e.g., `stabiliserGrille`, `detecterGroupesSimples`, `appliquerGravite`). This file contains most of the core algorithms.
- `input.c` — input handling and some client feature-flag demo code (DEMO macros). Some functions here operate as a separate utility/demo.
- `tableau.c`, `afficher.{c,h}` and `console.{c,h}` — auxiliary or demo/placeholder files. Many are stubs or sample code.

Important gaps & inconsistencies to be aware of (do not blindly fix without tests):
- Naming mismatch: `main.c` uses `GameState` + English functions, while `plantamitz.h` defines `EtatJeu` with French API names (e.g., `initialiserJeu`, `boucleJeu`). Keep language consistent; prefer `plantamitz.h` names as the project's definition of the public API, or reconcile both across files.
- Constants & types missing: `plantamiz.c` uses `ITEM_*`, `VIDE`, `NB_TYPES_ITEM` which are not defined in `plantamitz.h` yet. Before adding code, check for definitions and add them in the header.
- Multiple `main()` functions (e.g., `tableau.c`) can cause build/link conflicts: compile only the intended entrypoint `main.c`.
- Platform-specific console code (`Color()` and `_WIN32` blocks) are implemented in `plantamiz.c`. On Linux they are no-op; avoid editing platform stubs unless cross-platform behavior is needed.

Build & run:
- There is no Makefile; use the following commands for local development:

```bash
# Build the playable game (Linux / macOS)
# Include all relevant .c files and use the standard flag(s)
gcc -std=c99 -Wall -Wextra main.c plantamiz.c input.c -o plantamiz

# Run
./plantamiz

# Build and debug with symbols
gcc -g -O0 -std=c99 -Wall main.c plantamiz.c input.c -o plantamiz_debug
valgrind ./plantamiz_debug   # if valgrind is needed
```

Testing & validation tips
- There are no unit tests; prefer adding a small test harness in `tests/` for key algorithms (`detecterGroupesSimples`, `appliquerGravite`, `stabiliserGrille`). Example approach: implement a `test_grid.c` which sets up deterministic grids and asserts expected output.
- The `input.c` file has conditional `DEMO_ENABLE_RAPTOR_MINI` to run a client register/enable demo — compile with `-DDEMO_ENABLE_RAPTOR_MINI` to run the demo: `gcc -DDEMO_ENABLE_RAPTOR_MINI input.c -o demo_input`.

Design & implementation patterns
- Use `plantamitz.h` macros (`NB_LIGNES`, `NB_COLONNES`) consistently for grid dimensions.
- Board representation uses `char plateau[NB_LIGNES][NB_COLONNES]` with character constants for items and `VIDE` for empty cells. Functions should operate directly on the 2D array and use `int` loops (existing code uses `int i,j`).
- Group detection runs horizontal traversal followed by vertical traversal; prefer marking cells for removal and then apply `appliquerGravite` and `remplirAvecNouveauxItems`. Keep that stabilization loop as used in `stabiliserGrille`.

Conventions and style
- Prefer using the project's macro-based constants: `NB_LIGNES`, `NB_COLONNES`, `VIDE`, `ITEM_*`.
- Prefer `EtatJeu`/French API names (or reconcile entire repo to English) — do not mix both without an explicit rename PR.
- Use `char` for item types; keep wrapper functions `Color()` and other platform-specific functions in `plantamiz.c`.
- Use `-Wall -Wextra` when compiling to catch simple mistakes; prefer `-std=c99` or the project's chosen standard.

When generating code
- Search the repository for similar functions: copy the patterns used in `plantamiz.c`. For example, match detection uses two independent passes: horizontal then vertical; keep the same algorithmic structure.
- If adding or renaming symbols (e.g., `GameState` ⇄ `EtatJeu`), update all references across files and ensure `plantamitz.h` is authoritative.
- Add new helpers or tests in a small `tests/` folder; keep them independent from `main()` by using a testing entry point.
- If adding new constants (e.g., `ITEM_*`, `VIDE`, `NB_TYPES_ITEM`) add them to `plantamitz.h` and keep consistent naming (French uppercase macros).

Integration & external dependencies
- The project is self-contained C code; there are no external libraries. Windows console APIs are used behind _WIN32 guards.
- No build automation is present — add a `Makefile` or `CMakeLists.txt` if you add CI or build tasks.

Priorities for contributions
1. Reconcile naming and header/signature mismatches (e.g., `GameState` vs `EtatJeu`, `initGame` vs `initialiserJeu`).
2. Add missing constants / declarations to `plantamitz.h` and update `plantamiz.c` to use them.
3. Implement missing display/input stubs (`afficher.c`, `console.c`, `afficher.h`, `console.h`, `input.h`) with clear prototypes in `plantamitz.h` if needed.
4. Add a `Makefile` and unit test harness to validate algorithms.

If something looks broken
- Search for name mismatches (grep for `GameState` vs `EtatJeu`, and `initGame` vs `initialiserJeu`).
- If functions are missing from `plantamitz.h` but used elsewhere, add prototypes and keep naming consistent.

If you need help or can't find definitions
- Check `plantamiz.c` for algorithmic patterns; it is the best place to copy style/implementation ideas.
- Ask for clarification or a preferred naming scheme (English vs French) before mass rename refactors.

Quick actionable examples (copy/paste):
- Compile runnable game
```bash
gcc -std=c99 -Wall -Wextra main.c plantamiz.c input.c -o plantamiz && ./plantamiz
```

- Run input demo
```bash
gcc -DDEMO_ENABLE_RAPTOR_MINI input.c -o demo_input && ./demo_input
```

Thank you — if any part of this guidance is unclear or you'd like it to be stricter about naming conventions or style, tell me the desired approach and I will update this file.
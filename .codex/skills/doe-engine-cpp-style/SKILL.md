---
name: doe-engine-cpp-style
description: Enforce repository-specific C++17, CMake, PAROPTIM-style file headers, include guards, cross-platform rules, module boundaries, and incremental implementation practices for DOE_ENGINE. Use whenever creating, editing, reviewing, renaming, or organizing C++ headers, C++ sources, CMake files, Config classes, parsers, utilities, tests, or source directories in this repository.
---

# DOE Engine C++ Style

Apply the repository conventions before changing C++ or CMake code.

## Required workflow

1. Read [references/cpp-style.md](references/cpp-style.md) completely before editing C++ or CMake files.
2. Read `doc/architecture_and_module_boundaries.md` when a change affects module ownership, dependencies, runtime concepts, or directory structure.
3. Inspect the latest target files and preserve unrelated user changes.
4. Implement one cohesive layer at a time; do not generate multiple future layers merely because their names are known.
5. Perform static file inspection after editing. Do not compile, build, execute, or run tests unless the user explicitly requests it.

## Repository boundaries

- Keep `src/config` limited to declarative Config data types.
- Keep runtime data objects (Variable, Response, DesignPoint) constructed from Config entities in `src/runtime/data`.
- Keep runtime behavior modules (sampling, application, workflow, extractor, experiment) in `src/runtime`.
- Keep `persistence` as an optional interface layer in `src/persistence`; the system must run without a database implementation.
- Implement JSON Config parsers and loaders in `src/utils/json`.
- Implement error and logging components in `src/utils/log`.
- Keep Config objects declarative and free of runtime behavior.
- Keep sampling, Application execution, Workflow coordination, Extractor behavior, Experiment orchestration, and persistence in their documented modules.
- Do not recreate a broad `src/execution` module. Introduce `scheduler` or `process` only after a concrete need is approved.

## Change discipline

- Prefer small, reviewable patches.
- Use existing Fixtures as the serialized Config contract.
- Record unresolved model gaps instead of inventing fields silently.
- Keep Windows and Unix/Linux behavior equivalent through standard C++17 and CMake abstractions.

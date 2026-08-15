# DOE Engine C++ and CMake conventions

## File headers

Begin every newly created or materially regenerated `.h` and `.cc` file with this PAROPTIM-style header. Use the actual file name, a concise description, and the real creation date.

```cpp
/********************************************************************
 * "file_name.h" defines a concise description of this file.        *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: YYYY-MM-DD                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
```

Do not add this project header to vendored third-party code.

## Header guards

Use `#ifndef`, `#define`, and `#endif`; never use `#pragma once`.

Derive the macro from the repository-relative path, using uppercase letters and underscores:

```cpp
#ifndef DOE_ENGINE_CONFIG_APPLICATION_CONFIG_H_
#define DOE_ENGINE_CONFIG_APPLICATION_CONFIG_H_

// Declarations.

#endif  // DOE_ENGINE_CONFIG_APPLICATION_CONFIG_H_
```

Do not begin a guard with an underscore followed by an uppercase letter; that identifier is reserved by C++.

## Language and portability

- Require standard C++17 globally in CMake.
- Set `CMAKE_CXX_STANDARD_REQUIRED` to `ON` and `CMAKE_CXX_EXTENSIONS` to `OFF`.
- Prefer the C++ standard library over platform APIs.
- Represent resolved local paths with `std::filesystem::path`.
- Keep unexpanded runtime path templates as strings until placeholders are resolved.
- Do not manually join paths with `\\` or `/`.
- Preserve command arguments as separate strings; do not build a shell command by concatenation.
- Do not assume an executable has an `.exe` suffix.
- Account for path spaces, Unicode paths, `LF`, and `CRLF`.
- Isolate unavoidable Windows/POSIX process differences behind one interface.

## CMake

Keep these project-level settings:

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Add `src` and all current recursive `src` directories to target include paths so VSCode/CMake Tools can resolve headers during code browsing. Keep include paths target-scoped; do not use global `include_directories()`.

Use CMake targets and target properties. Do not generate IDE-specific project files manually.

## Includes and declarations

- Include the matching project header first in a `.cc` file.
- Include a header in the same directory by file name only, such as `"application_config.h"`; do not prepend its current directory name.
- Use a directory-qualified project include only when the included header is in another directory.
- List project headers before standard-library headers, separated by one blank line.
- Include what each header directly uses; do not depend on transitive includes.
- Forward-declare only when it materially reduces coupling and remains clear.
- Keep all DOE Engine code under the `doe` namespace and module-specific nested namespaces.

## Naming and formatting

- Use `PascalCase` for types and public factory/parser functions already following that convention.
- Use Google C++ scoped-enum naming with a `k` prefix, such as `ValueType::kReal` and `ApplicationType::kCommand`.
- Use `snake_case` for Config data fields so names correspond clearly to JSON fields.
- Use a trailing underscore for private runtime data members.
- Put function and class/struct opening braces on the next line when adding new implementation code, following PAROPTIM layout.
- Keep comments concise and explain contracts or reasons, not obvious syntax.

## Config and parser rules

- Treat `tests/fixtures` as executable examples of serialized Config objects.
- Keep Config types as declarative data; do not read files, launch processes, or write databases from them.
- Implement Config parsing in `src/utils/json`, using the vendored `third_party/nlohmann/json.hpp`.
- Keep JSON library types out of public Config headers.
- Validate required fields during parsing; do not rely on default member values to accept missing JSON fields.
- Separate single-file structural validation from cross-file semantic validation.
- Include the source file and JSON field location in configuration errors.
- Resolve a referenced JSON file relative to the JSON file containing that reference.
- Match Extractor instance binding keys exactly to the ports declared by its Extractor definition.
- Require an absolute JSON file path at each public Parser entry point.
- Put reusable JSON access, type conversion, field validation, and deserialization in `src/utils/json`.
- Put future error and logging components in `src/utils/log`; do not recreate `config_error.h`.

## Module boundaries

Use `doc/architecture_and_module_boundaries.md` as the authority for module ownership. In particular:

- `config`: declarative Config data types (JSON to Config entities).
- `runtime/data`: runtime data objects constructed from Config entities (Variable, Response, DesignPoint).
- `runtime`: runtime behavior modules (sampling, application, workflow, extractor, experiment).
- `utils/json`: JSON parsing, Config deserialization, and configuration file loading.
- `utils/log`: error and logging components.
- `sampling`: Design Point generation.
- `application`: input preparation, external process execution, and output checks.
- `workflow`: Application ordering and connections.
- `extractor`: response extraction.
- `experiment`: top-level DOE orchestration.
- `persistence`: optional interface layer that records system behavior; the system must run without it.

Do not move responsibilities into a generic helper merely to avoid deciding their owner.

## Verification policy

After edits, inspect files statically for guards, headers, include paths, duplicate declarations, and stale references. Do not compile, configure CMake, execute binaries, or run tests unless the user explicitly asks for that verification.

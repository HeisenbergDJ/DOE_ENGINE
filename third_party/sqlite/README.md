# SQLite (vendored)

This directory holds the vendored SQLite amalgamation. Download the
SQLite amalgamation from <https://www.sqlite.org/download.html>
(e.g. `sqlite-amalgamation-3450200.zip`), extract it, and place these two files
here:

- `sqlite3.c`
- `sqlite3.h`

The build (`doe_sqlite` target) compiles `sqlite3.c` as C and links it into the
engine's persistence layer. Do not edit the amalgamation files; keep them as
shipped by SQLite.

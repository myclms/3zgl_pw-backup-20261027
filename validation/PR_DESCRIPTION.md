### Task Description

Fixes #1469.

Shared DDL embeds raw column names between backticks. Backticks inside a valid identifier therefore produce invalid SQL, including during ENUM/SET unique-index backfill. Calling `sql::ObSQLUtils` from this shared code would introduce the upward dependency described in the issue.

### Solution Description

Quote identifiers with a file-local Share helper that doubles embedded backticks, using only the existing OBLib string API. Apply it to original names, ENUM/SET unsigned casts and aliases, including the shadow-column prefix. Preserve existing output for ordinary names and stop immediately on append failures.

Add `ddl.ddl_identifier_escaping` to mysqltest with its expected result and legacy registration. It covers backtick-containing primary keys, ENUM and SET names, adjacent backticks, Unicode, ordinary-name controls, NULL rows and successful unique-index registration.

### Validation

- Reproduced five `1064 Parse error` failures on the official Windows seekdb 1.4.0.0 runtime. The ordinary-identifier control builds its index successfully.
- Ran all 18 fixture statements with ordinary identifiers on that same runtime. All five unique indexes build and data/NULL results match expectations.
- Compiled the actual old/new production formatter bodies with minimal ObString/ObSqlString adapters, using Zig 0.13.0's Clang frontend, C++17, `-Wall -Wextra -Werror -fsanitize=undefined -fsanitize-trap=undefined`. 8,512 formatter cases and 268,532 assertions pass, including length-delimited input and every append-failure position. 5,472 backtick cases differ from the old formatter. This is a focused harness, not a full server build.
- `git diff --cached --check` passes. No SQL-layer include or call is added to Share.

The complete patched server and the actual mysqltest executable have not been built or run locally; CI integration validation remains pending. The SQL expected result is checked in but has not been recorded from a patched engine.

### Scope

Based on upstream master `4b28bcbff0a476b25f2f668c2560a1b77b2095e5`. This implements the lower-layer alternative on current master; it does not include the separate vector-rebuild cleanup changes in #1442. No persisted format or public SQL interface changes are intended.

AI assistance was used for investigation, implementation and focused validation.

### Release Note

Escape embedded backticks when shared DDL generates column identifiers, without introducing a dependency on the SQL module.

OB-P:Bcl1kH2tJcwXqfpfmeKkjA

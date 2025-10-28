# MDSL Parser Combinator Tests

Unit tests for the parser combinator library with TokenStream support.

## Building and Running

```bash
cmake -B build
cmake --build build
cd build/tests
./test_combinators
```

Or using CTest:

```bash
cd build
ctest
```

## Test Coverage

### Basic Combinators

- **streamToken**: Matches a single token of a specific type
- **orElse**: Tries alternatives, backtracking on failure
- **map**: Transforms parser results

### Repetition Combinators

- **streamMany**: Matches zero or more occurrences
- **streamMany1**: Matches one or more occurrences
- **streamOptional**: Matches zero or one occurrence
- **streamSepBy**: Matches items separated by delimiters

### Grammar Patterns Tested

1. **Token Matching**: Basic token recognition
2. **Alternatives**: Choice between multiple parsers with backtracking
3. **Repetition**: Zero-or-more and one-or-more patterns
4. **Optional Elements**: Handling optional syntax
5. **Separated Lists**: Comma-separated values, parameter lists
6. **Sequences**: Multiple parsers in order
7. **Nested Alternatives**: Complex choice structures
8. **Backtracking**: Parser state restoration on failure

## TokenStream vs ILexer

The `StreamParser` combinators use `TokenStream` instead of `ILexer`:

- **TokenStream**: Pre-tokenizes input into immutable vector, safe backtracking via position tracking
- **ILexer**: Direct lexer access with checkpoint/restore

Use `StreamParser` with `TokenStream` for:
- Testing parsers without complex lexer setup
- Guaranteed safe backtracking
- Better performance with repeated backtracking

Use `Parser` with `ILexer` for:
- Streaming large inputs
- Tight integration with lexer state

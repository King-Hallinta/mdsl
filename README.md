# MDSL

**Framework for building custom languages**

MDSL is a library that helps you build domain-specific languages. It provides all the essential components, from tokenization to code generation.

## Features

### Lexer
- Configurable tokenization with customizable rules
- Keyword registration and identifier customization
- Built-in support for common token types
- Line/block comment handling
- String and numeric literal parsing

### Parser
- Recursive descent parser base class
- Parser combinator support
- AST node generation
- Grammar definition utilities
- Error recovery mechanisms

### Semantic Analysis
- Symbol table management
- Type system and type checking
- Scope handling
- Semantic validation

### Intermediate Representation (IR)
- SSA-form IR generation
- IR builder with fluent API
- Module and function organization
- Basic block management
- IR pass framework

### Code Generation
- IR to code emission
- C++ code generator
- Bytecode emitter
- Template engine for code generation
- Extensible backend support

## Building

### Requirements
- C++20 compatible compiler (MSVC 2019+, GCC 10+, Clang 12+)
- CMake 3.10 or higher

### Build Instructions

```bash
cmake -B build
cmake --build build
```

### Integration

```cmake
add_subdirectory(mdsl)
target_link_libraries(your_target PRIVATE MDSL)
```

## Usage

```cpp
#include <MDSL.h>

using namespace mdsl;

// Create lexer
auto lexer = std::make_shared<lexer::Lexer>(sourceCode);
lexer->AddKeyword("if", lexer::TokenType::UserDefinedStart);

// Create parser
class MyParser : public parser::RecursiveDescentParser
{
public:
    MyParser(std::shared_ptr<lexer::ILexer> lex)
    {
        lexer = lex;
        currentToken = lexer->NextToken();
    }

    // Implement parsing logic
};

// Parse and generate IR
MyParser parser(lexer);
auto ast = parser.Parse();

// Generate code
ir::IRModule module("my_module");
// ... IR generation
```

## Project Structure

```
mdsl/
├── include/
│   ├── MDSL.h              # Main header
│   └── mdsl/
│       ├── core/           # Core utilities
│       ├── lexer/          # Tokenization
│       ├── parser/         # Parsing
│       ├── semantic/       # Analysis
│       ├── ir/             # IR generation
│       ├── codegen/        # Code emission
│       └── util/           # Utilities
└── src/                    # Implementation
```

## License

MIT License - See [LICENSE](LICENSE) for details

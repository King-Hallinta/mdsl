# MDSL

**Framework for building custom languages**

MDSL gives you the parts needed to build your own programming language or DSL.

What you can build:
- Scripting languages
- Configuration languages
- Query languages
- Template engines
- Expression evaluators

## What's Included

**Lexer** - Turns source text into tokens. Register keywords, handle strings/numbers, skip comments.

**Parser** - Builds abstract syntax trees from tokens. Recursive descent base class included.

**Semantic Analysis** - Symbol tables, type checking, scope management.

**IR Generation** - Convert AST to SSA-form intermediate representation.

**Code Generation** - Emit final code. Ships with C++ and bytecode backends. Write your own by extending CodeEmitter.

## Building

Requires C++20 compiler and CMake 3.10+

```bash
cmake -B build
cmake --build build
```

Add to your CMake project:

```cmake
add_subdirectory(mdsl)
target_link_libraries(your_target PRIVATE MDSL)
```

## Usage

### Basic Pipeline

```cpp
#include <mdsl.h>
using namespace mdsl;

std::string source = "x = 10 + 20";

auto lexer = std::make_shared<lexer::Lexer>(source);
lexer->AddKeyword("let", lexer::TokenType::UserDefinedStart);

ir::IRModule module("example");
ir::IRBuilder builder;
builder.SetModule(&module);

auto func = module.CreateFunction("main");
auto entry = func->CreateBasicBlock("entry");
builder.SetInsertPoint(entry);

auto c1 = builder.CreateConstant(10);
auto c2 = builder.CreateConstant(20);
auto result = builder.CreateAdd(c1, c2);

codegen::CppEmitter emitter;
auto code = emitter.Emit(module);
```

### Example: Tokenizing Source Code

```cpp
#include <mdsl.h>
#include <iostream>

int main()
{
    std::string source = R"(
        x = 10
        y = 20
        result = x + y
    )";

    mdsl::lexer::Lexer lexer(source);
    lexer.AddKeyword("let", mdsl::lexer::TokenType::UserDefinedStart);
    lexer.AddKeyword("var", mdsl::lexer::TokenType::UserDefinedStart + 1);

    mdsl::lexer::Token token;
    while (!lexer.IsAtEnd())
    {
        token = lexer.NextToken();
        if (token.type == mdsl::lexer::TokenType::EndOfFile)
            break;

        std::cout << mdsl::lexer::TokenTypeToString(token.type)
                  << ": " << token.lexeme << "\n";
    }

    return 0;
}
```

### Creating Custom Code Generators

```cpp
#include <mdsl.h>
#include <sstream>

std::string EmitCustomBackend(const mdsl::ir::IRModule& module)
{
    std::stringstream output;

    for (const auto& func : module.GetFunctions())
    {
        output << "function " << func->GetName() << ":\n";

        for (const auto& block : func->GetBasicBlocks())
        {
            output << "  " << block->GetName() << ":\n";

            for (const auto& instr : block->GetInstructions())
            {
                output << "    %" << instr->GetId() << " = ";
                output << mdsl::ir::IRInstruction::OpcodeToString(instr->GetOpcode());
                output << "\n";
            }
        }
    }

    return output.str();
}
```

### Working with Types

```cpp
#include <mdsl.h>

mdsl::semantic::TypeRegistry registry;

auto intType = std::make_shared<mdsl::semantic::PrimitiveType>(
    mdsl::semantic::TypeKind::Integer, "int");
auto floatType = std::make_shared<mdsl::semantic::PrimitiveType>(
    mdsl::semantic::TypeKind::Float, "float");

registry.RegisterType(intType);
registry.RegisterType(floatType);

auto arrayType = std::make_shared<mdsl::semantic::ArrayType>(intType, 10);
auto ptrType = std::make_shared<mdsl::semantic::PointerType>(intType);

if (intType->Equals(floatType.get()))
{
    // Types are equal
}
```

## Project Structure

```
mdsl/
  include/
    mdsl.h              # Main header
    mdsl/
      core/             # Core utilities
      lexer/            # Tokenization
      parser/           # Parsing
      semantic/         # Analysis
      ir/               # IR generation
      codegen/          # Code emission
      util/             # Utilities
  src/                  # Implementation
```

## License

MIT License - See [LICENSE](LICENSE) for details
